/**
 * @file page_table.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-09-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "page_table.h"
#include <stdlib.h>

page_table_lvl_1_t *new_page_table(size_t rows)
{
	page_table_lvl_1_t *table = NULL;
	table = malloc(sizeof(page_table_lvl_1_t) * rows);

	// Set the second page ID to invalid
	for (uint32_t i = 0; i < rows; i++)
	{
		table[i].second_page = UINT32_MAX;
	}

	return table;
}

page_table_lvl_2_t *new_page_table_lvl2(size_t rows)
{
	page_table_lvl_2_t *table = NULL;
	table = malloc(sizeof(page_table_lvl_2_t) * rows);

	// Set the FRAME ID to invalid
	for (uint32_t i = 0; i < rows; i++)
	{
		table[i].frame = UINT32_MAX;
		table[i].modified = false;
		table[i].present = false;
		table[i].use = false;
	}

	return table;
}
