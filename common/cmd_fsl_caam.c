/*
 * Copyright (C) 2012-2016 Freescale Semiconductor, Inc.
 *
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <common.h>
#include <command.h>
#include <fsl_caam.h>

static int do_caam(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret, i;
	void *data_addr;
	void *blob_addr;
	int size;

	if (argc != 5)
		return CMD_RET_USAGE;

	data_addr = (void *)strict_strtoul(argv[2], NULL, 16);
	blob_addr = (void *)strict_strtoul(argv[3], NULL, 16);
	size      = strict_strtoul(argv[4], NULL, 16);

	caam_open();

	if (strcmp(argv[1], "genblob") == 0) {
		ret = caam_gen_blob((uint32_t)data_addr, (uint32_t)blob_addr, size);
		if (ret != SUCCESS) {
			printf("Error during blob decap operation: 0x%d\n",ret);
			return CMD_RET_FAILURE;
		}

		/* Print the generated DEK blob */
		printf("DEK blob is available at 0x%08X and equals:\n",
			(unsigned int)blob_addr);
		for (i = 0; i < size; i++)
			printf("%02X ", ((uint8_t *)blob_addr)[i]);
		printf("\n\n");

		return CMD_RET_SUCCESS;
	} else if (strcmp(argv[1], "decap") == 0) {
		if (size <= 48)
			return CMD_RET_USAGE;

		ret = caam_decap_blob(data_addr, blob_addr, (uint32_t)size);
		if (ret != SUCCESS)
			printf("Error during blob decap operation: 0x%d\n",
				ret);
		else {
			printf("Blob decap at SM PAGE1 original data is:\n");
			for (i = 0; i < size; i++) {
				printf("0x%x  ",
					*(unsigned char *)(data_addr+i));
				if (i % 16 == 0)
					printf("\n");
			}
			printf("\n");
		}

		return CMD_RET_SUCCESS;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	caam, 5, 1, do_caam,
	"Freescale i.MX CAAM command for data blob enc/de-capsulation",
	"genblob data_addr blob_addr data_size\n \
	caam decap blobaddr data_addr data_size\n \
	\n "
);