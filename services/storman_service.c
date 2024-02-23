/******************************************************************************
 * @file storman_service.c
 *
 * @brief storagemanager service implementation and FreeRTOS task creation.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 02-14-2024
 * 
 * @copyright Copyright (c) 2024 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 * 
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <string.h>
#include <lfs.h>
#include "hardware_config.h"
#include "rtos_utils.h"
#include "shell.h"
#include "services.h"
#include "service_queues.h"
#include "hardware/flash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static void prvStorageManagerTask(void *pvParameters);
TaskHandle_t xStorManTask;

extern void shell_mnt_mount(void); // declared in this file so /mnt can be mounted on-the-fly

struct storman_item_t smi_glob; // write any resulting lfs data into global struct so it can be used by other tasks
SemaphoreHandle_t smi_glob_sem; // binary semaphore used to provide a data ready signal to other tasks
int err = 0; // used throughout the task to indicate that an error has occured

// main service function, creates FreeRTOS task from prvStorageManagerTask
BaseType_t storman_service(void)
{
    BaseType_t xReturn;

    // create the FreeRTOS task
    xReturn = xTaskCreate(
        prvStorageManagerTask,
        xstr(SERVICE_NAME_STORMAN),
        STACK_STORMAN,
        NULL,
        PRIORITY_STORMAN,
        &xStorManTask
    );

    // print timestamp value
    cli_uart_puts(timestamp());

    if (xReturn == pdPASS) {
        cli_uart_puts("Storage manager service running\r\n");
    }
    else {
        cli_uart_puts("Error starting the storage manager service\r\n");
    }

    return xReturn;
}

// FreeRTOS task created by storman_service
static void prvStorageManagerTask(void *pvParameters)
{
    // littlefs filesystem variables
    lfs_t lfs_flash0;
    lfs_file_t flash0_file;
    lfs_dir_t flash0_dir;
    
    // littlefs flash0 filesystem configuration
    struct lfs_config fs_config_flash0 = {
        // block device operation HAL functions
        .read = onboard_flash_read,
        .prog = onboard_flash_write,
        .erase = onboard_flash_erase,
        .sync = onboard_flash_sync,

        // block device configuration
        .read_size = 1,
        .prog_size = FLASH_PAGE_SIZE,
        .block_size = FLASH_SECTOR_SIZE,
        .block_count = FLASH0_FS_SIZE / FLASH_SECTOR_SIZE,
        .block_cycles = 500,
        .cache_size = FLASH_PAGE_SIZE,
        .lookahead_size = 32,

        // other configurations
        .name_max = PATHNAME_MAX_LEN, // max length of path+filenames
        .file_max = FILE_SIZE_MAX,    // max size of a file in bytes
    };

    // print some NVM initialization text
    cli_uart_puts(timestamp());
    cli_uart_puts("Initializing NVM...");
    // mount the flash0 filesystem
    if(lfs_mount(&lfs_flash0, &fs_config_flash0) == 0) {
        shell_mnt_mount(); // create '/mnt' node in the shell
        cli_uart_puts("flash0 mounted in /mnt\r\n");
    } else {
        // if non-zero, format and try again
        cli_uart_puts("no filesystem found\r\n");
        cli_uart_puts(timestamp());
        cli_uart_puts("Formatting storage...\r\n"); // should only happen on first mount
        if(lfs_format(&lfs_flash0, &fs_config_flash0) == 0) {
            cli_uart_puts(timestamp());
            cli_uart_puts("formatting complete - ");
            if(lfs_mount(&lfs_flash0, &fs_config_flash0) == 0) { // retry mount
                cli_uart_puts("flash0 mounted in /mnt\r\n");
            } else {
                cli_uart_puts("problem mounting flash0!\r\n");
            }
        } else {
            cli_uart_puts(timestamp());
            cli_uart_puts("problem formatting flash0!\r\n");
        }
    }

    // create the binary semaphore used to signal other tasks that data is ready
    smi_glob_sem = xSemaphoreCreateBinary();
    
    while(true) {
        err = 0;

        // Check the storagemanager queue to see if an item is available
        if (xQueueReceive(storman_queue, (void *)&smi_glob, 0) == pdTRUE)
        {
            // determine what action to perform in the filesystem.
            // note that there may be further littlefs capabilities which are
            // not implemented here. see "littlefs/lfs.h" for all APIs
            switch(smi_glob.action)
            {
                case LSDIR:      // list directory contents
                    err = lfs_dir_open(&lfs_flash0, &flash0_dir, smi_glob.sm_item_name);
                    if (err == 0) {
                        char *lsdir_output = (char*)pvPortMalloc(BUF_OUT_SIZE); // use FreeRTOS malloc and free, it is a large buffer
                        strcpy(lsdir_output, USH_SHELL_FONT_STYLE_BOLD
                                             USH_SHELL_FONT_COLOR_BLUE
                                             "File List\r\n"
                                             "---------\r\n"
                                             USH_SHELL_FONT_STYLE_RESET);
                        while(lfs_dir_read(&lfs_flash0, &flash0_dir, &smi_glob.sm_item_info) > 0) {
                            strcpy(lsdir_output + strlen(lsdir_output), smi_glob.sm_item_info.name);
                            if (smi_glob.sm_item_info.type == LFS_TYPE_DIR) { // check if this item is a directory
                                strcpy(lsdir_output + strlen(lsdir_output), "/"); // append a forward slash to the name if dir
                            }
                            strcpy(lsdir_output + strlen(lsdir_output), "\r\n");
                        };
                        strcpy(smi_glob.sm_item_data, lsdir_output); // use the global data to store a copy of the combined list output
                        smi_glob.sm_item_data[strlen(smi_glob.sm_item_data)] = 0; // put a null character at the end
                        xSemaphoreGive(smi_glob_sem); // provide the binary semaphore to indicate data is available
                        vPortFree(lsdir_output);
                        err = lfs_dir_close(&lfs_flash0, &flash0_dir);
                    }
                    break;
                case MKDIR:      // create a new directory
                    err = lfs_mkdir(&lfs_flash0, smi_glob.sm_item_name);
                    break;
                case RMDIR:      // delete a directory (only if empty)
                    // note: currently the same as RMFILE but kept separate for future enhancement (delete non-empty dirs, etc)
                    err = lfs_remove(&lfs_flash0, smi_glob.sm_item_name);
                    break;
                case MKFILE:     // create a new empty file (if file already exists, will generate error)
                    err = lfs_file_open(&lfs_flash0, &flash0_file, smi_glob.sm_item_name, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_EXCL);
                    if (err < 0) break;
                    err = lfs_file_write(&lfs_flash0, &flash0_file, NULL, (lfs_size_t)1); // may not be needed?
                    if (err < 0) break;
                    err = lfs_file_close(&lfs_flash0, &flash0_file);
                    break;
                case RMFILE:     // delete a file
                    err = lfs_remove(&lfs_flash0, smi_glob.sm_item_name);
                    break;
                case DUMPFILE:   // dump entire contents of file
                    err = lfs_file_open(&lfs_flash0, &flash0_file, smi_glob.sm_item_name, LFS_O_RDONLY);
                    if (err < 0) break;
                    err = lfs_file_read(&lfs_flash0, &flash0_file, smi_glob.sm_item_data, sizeof(smi_glob.sm_item_data));
                    if (err < 0) break;
                    smi_glob.sm_item_data[lfs_file_size(&lfs_flash0, &flash0_file)] = 0; // put a null character at the end of the file contents data
                    xSemaphoreGive(smi_glob_sem); // provide the binary semaphore to indicate data is available
                    err = lfs_file_close(&lfs_flash0, &flash0_file);
                    break;
                case READFILE:   // read a portion of a file
                    err = lfs_file_open(&lfs_flash0, &flash0_file, smi_glob.sm_item_name, LFS_O_RDONLY);
                    if (err < 0) break;
                    err = lfs_file_seek(&lfs_flash0, &flash0_file, smi_glob.sm_item_offset, LFS_SEEK_SET);
                    if (err < 0) break;
                    err = lfs_file_read(&lfs_flash0, &flash0_file, smi_glob.sm_item_data, smi_glob.sm_item_size);
                    if (err < 0) break;
                    smi_glob.sm_item_data[smi_glob.sm_item_size] = 0; // put a null character at the end of the read data
                    xSemaphoreGive(smi_glob_sem); // provide the binary semaphore to indicate data is available
                    err = lfs_file_close(&lfs_flash0, &flash0_file);
                    break;
                case WRITEFILE:  // write to a new file, or overwrite an existing file
                    err = lfs_file_open(&lfs_flash0, &flash0_file, smi_glob.sm_item_name, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
                    if (err < 0) break;
                    err = lfs_file_write(&lfs_flash0, &flash0_file, smi_glob.sm_item_data, (lfs_size_t)strlen(smi_glob.sm_item_data));
                    if (err < 0) break;
                    err = lfs_file_close(&lfs_flash0, &flash0_file);
                    break;
                case APPENDFILE: // append data to end of existing file
                    err = lfs_file_open(&lfs_flash0, &flash0_file, smi_glob.sm_item_name, LFS_O_WRONLY | LFS_O_APPEND);
                    if (err < 0) break;
                    err = lfs_file_write(&lfs_flash0, &flash0_file, smi_glob.sm_item_data, (lfs_size_t)strlen(smi_glob.sm_item_data));
                    if (err < 0) break;
                    err = lfs_file_close(&lfs_flash0, &flash0_file);
                    break;
                case FILESTAT:   // get file statistics (size)
                    err = lfs_stat(&lfs_flash0, smi_glob.sm_item_name, &smi_glob.sm_item_info);
                    if (err < 0) break;
                    strcpy(smi_glob.sm_item_data, smi_glob.sm_item_info.name);
                    sprintf(smi_glob.sm_item_data + strlen(smi_glob.sm_item_data), ": %lu bytes", smi_glob.sm_item_info.size);
                    xSemaphoreGive(smi_glob_sem); // provide the binary semaphore to indicate data is available
                    break;
                case FSSTAT:     // get filesystem statistics
                    smi_glob.sm_item_size = lfs_fs_size(&lfs_flash0);
                    if (smi_glob.sm_item_size < 0) {
                        err = smi_glob.sm_item_size;
                        break;
                    }
                    // format output for printing elsewhere
                    sprintf(smi_glob.sm_item_data, "Filesystem usage: %lu/%lu blocks (%lu/%lu bytes)",
                                                   smi_glob.sm_item_size,
                                                   FLASH0_FS_SIZE/FLASH0_BLOCK_SIZE,
                                                   smi_glob.sm_item_size*FLASH0_BLOCK_SIZE,
                                                   FLASH0_FS_SIZE);
                    xSemaphoreGive(smi_glob_sem); // provide the binary semaphore to indicate data is available
                    break;
                case FORMAT:     // format the filesystem (erase all contents)
                    if(lfs_format(&lfs_flash0, &fs_config_flash0) == 0 && lfs_mount(&lfs_flash0, &fs_config_flash0)  == 0) {
                        strcpy(smi_glob.sm_item_data, "formatting complete");
                    }
                    else {
                        strcpy(smi_glob.sm_item_data, "problem formatting");
                    }
                    xSemaphoreGive(smi_glob_sem); // provide the binary semaphore to indicate data is available
                    break;
                default:
                    break;
            }
        }

        // check if there were any lfs errors and print to CLI
        // error definitions come from enum lfs_error in lfs.h
        if (err < 0) {
            char err_msg[48] = "filesystem error: ";
            switch (err)
            {
            case LFS_ERR_IO:
                strcpy(err_msg + strlen(err_msg), "error during device operation");
                break;
            case LFS_ERR_CORRUPT:
                strcpy(err_msg + strlen(err_msg), "corrupted");
                break;
            case LFS_ERR_NOENT:
                strcpy(err_msg + strlen(err_msg), "entry does not exist");
                break;
            case LFS_ERR_EXIST:
                strcpy(err_msg + strlen(err_msg), "entry already exists");
                break;
            case LFS_ERR_NOTDIR:
                strcpy(err_msg + strlen(err_msg), "entry is not a dir");
                break;
            case LFS_ERR_ISDIR:
                strcpy(err_msg + strlen(err_msg), "entry is a dir");
                break;
            case LFS_ERR_NOTEMPTY:
                strcpy(err_msg + strlen(err_msg), "dir is not empty");
                break;
            case LFS_ERR_BADF:
                strcpy(err_msg + strlen(err_msg), "bad file number");
                break;
            case LFS_ERR_FBIG:
                strcpy(err_msg + strlen(err_msg), "file too large");
                break;
            case LFS_ERR_INVAL:
                strcpy(err_msg + strlen(err_msg), "invalid parameter");
                break;
            case LFS_ERR_NOSPC:
                strcpy(err_msg + strlen(err_msg), "no space left on device");
                break;
            case LFS_ERR_NOMEM:
                strcpy(err_msg + strlen(err_msg), "no more memory available");
                break;
            case LFS_ERR_NOATTR:
                strcpy(err_msg + strlen(err_msg), "no data/attr available");
                break;
            case LFS_ERR_NAMETOOLONG:
                strcpy(err_msg + strlen(err_msg), "file name too long");
            default:
                strcpy(err_msg + strlen(err_msg), "unknown");
                break;
            }
            cli_print_raw(err_msg);
        }

        // update this task's schedule
        task_sched_update(REPEAT_STORMAN, DELAY_STORMAN);
    }
}