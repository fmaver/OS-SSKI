/**
 * @file opcode.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief Operation codes definitions
 * @version 0.1
 * @date 03-26-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

/**
 * @brief Operation Code enumeration.
 *
 */
typedef enum Opcode
{
	// Disconnected - Desconectado
	DC,
	// Message - mensaje estándar
	MSG,
	// Package - paquete
	PKG,
	// System Call - syscall
	SYS,
	// Command - instruccion
	CMD,
	// I/O - Return
	INOUT,
	// operands
	OP,
	// PCB
	PCB,
	// Read
	RD,
	// Write
	WR,
	// SWAP PCB
	SWAP_PCB,
	// FETCH SWAPPED PCB
	RETRIEVE_SWAPPED_PCB,
	// FETCH_SWAPPED_PCB RESPONSE
	RETRIEVED_PCB,
	// WRITE
	WT,
	// Interruption
	INT,
	// Page Size
	SZ,
	// Amount of entries per page
	ENTRIES,
	// Value of the Frame
	FRAME,
	// Second Level Page
	SND_PAGE,
	// Memory for new Process
	MEMORY_INIT,
	// Process ends signal
	PROCESS_TERMINATED
} opcode_t;

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

/**
 * @brief Obtains the name of a operation code.
 *
 * @param opcode the opcode number
 * @return the label for the operation code
 */
char *opcode_to_string(opcode_t opcode);
