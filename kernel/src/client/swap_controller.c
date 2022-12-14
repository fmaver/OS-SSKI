/**
 * @file swap_controller.c
 * @author Franco Parente (fparente14@frba.utn.edu.ar)
 * @brief Swap entre Memoria y Disco
 * @version 0.1
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "swap_controller.h"
#include "pcb.h"
#include "kernel.h"

// The global Kernel module object.
extern kernel_t g_kernel;

#define LOG_PCB(pcb)                                                                                                                \
	{                                                                                                                               \
		LOG_INFO("[SWAP-Controller] :=> PCB<%d>(size: %lu, estimation: %d, pc: %d)", pcb->id, pcb->size, pcb->estimation, pcb->pc); \
	}

ssize_t
swap_controller_send_pcb(opcode_t opcode, pcb_t *pcb)
{
	ssize_t bytes_sent = -1;
	void *stream = pcb_to_stream(pcb);
	LOG_WARNING("[SWAP-Controller] :=> Sending SWAP for PCB...");

	LOG_PCB(pcb);

	bytes_sent = conexion_enviar_stream(g_kernel.conexion_memory, opcode, stream, pcb_bytes_size(pcb));

	free(stream);

	return bytes_sent;
}

ssize_t
swap_controller_request_pcb(uint32_t pid)
{
	LOG_TRACE("[SWAP-Controller] :=> Requesting PCB #%d", pid);
	return conexion_enviar_stream(g_kernel.conexion_memory, RETRIEVE_SWAPPED_PCB, &pid, sizeof(pid));
}

void *
swap_controller_receive_pcb(void)
{
	// Recover Stream from Connection
	ssize_t bytes_received = 0;
	void *stream = NULL;
	stream = conexion_recibir_stream(g_kernel.conexion_memory.socket, &bytes_received);
	LOG_WARNING("[SWAP-Controller] :=> SWAP metadata received [%ld bytes]", bytes_received);

	// Recover data from Stream
	// pcb_t *recovered_pcb = pcb_from_stream(stream);
	// LOG_PCB(recovered_pcb);

	// Deallocate Stream
	free(stream);

	return NULL;
}

ssize_t swap_controller_exit(pcb_t *pcb)
{
	void *stream = NULL;

	opcode_t pcb_terminated = PROCESS_TERMINATED;
	uint32_t pid = pcb->id;
	uint32_t page_table = pcb->page_table;
	uint32_t size = sizeof(pid) + sizeof(page_table);

	stream = malloc(size);
	uint32_t offset = 0;
	memcpy(stream, &pid, sizeof(pid));
	offset += sizeof(pid);
	memcpy(stream + offset, &page_table, sizeof(page_table));

	ssize_t ret = conexion_enviar_stream(g_kernel.conexion_memory, pcb_terminated, stream, size);
	LOG_TRACE("[SWAP-Controller] :=> Request sent [%ld bytes]", ret);
	free(stream);
	return ret;
}
