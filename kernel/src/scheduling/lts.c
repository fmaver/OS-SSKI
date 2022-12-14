/**
 * @file lts.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <math.h>
#include <stdlib.h>
#include "kernel.h"
#include "pcb_unit.h"
#include "safe_queue.h"
#include "log.h"
#include "accion.h"
#include "mts.h"
#include "cpu_controller.h"
#include "operands.h"

// ============================================================================================================
//                                   ***** Declarations *****
// ============================================================================================================

void admit(kernel_t *kernel);

// ============================================================================================================
//                                   ***** Definitions *****
// ============================================================================================================

void *
long_term_schedule(void *kernel_ref)
{
	LOG_TRACE("[LTS] :=> Initializing Long Term Scheduler");

	kernel_t *kernel = kernel_ref;
	scheduler_t sched = kernel->scheduler;
	int dom = -1, request = -99;

	for (;;)
	{
		// Wait for a process to be created.
		sem_getvalue(sched.req_admit, &request);
		LOG_WARNING("[LTS] :=> Waiting for a process...");
		WAIT(sched.req_admit);
		LOG_WARNING("[LTS] :=> There are <%d> previous requests", request);
		LOG_TRACE("[LTS] :=> Verifying Multiprogramming grade...");
		sem_getvalue(sched.dom, &dom);

		if (dom > 0)
		{
			LOG_INFO("[LTS] :=> Available Slots: [%d] -> Admitting...", dom);
		}
		else
		{
			LOG_ERROR("[LTS] :=> No available slots - Already Queued: %d", abs(dom));
		}

		// Wait for programs to end...
		WAIT(sched.dom);
		sem_getvalue(sched.dom, &dom);
		admit(kernel);
		LOG_WARNING("[LTS] :=> Updated available slots <%d>", dom);
	}

	return NULL;
}

void admit(kernel_t *kernel)
{

	safe_queue_t *new = kernel->scheduler.new;
	safe_queue_t *ready = kernel->scheduler.ready;
	conexion_t memory = kernel->conexion_memory;
	operands_t operands;

	if (new != NULL && ready != NULL)
	{
		LOG_TRACE("[LTS] :=> Admitting a process...");
		// Sets to ready a new process and enqueues.
		pcb_t *pcb = resume(&kernel->scheduler);

		// When cannot resume a process - A new one must be instead.
		if (pcb == NULL)
		{
			pcb = safe_queue_pop(new);
			LOG_TRACE("[LTS] :=> New process admitted");

			// Request page table.
			if (conexion_esta_conectada(memory))
			{
				LOG_TRACE("[LTS] :=> Request page table...");

				uint32_t pcb_id = pcb->id;
				uint32_t pcb_size = pcb->size;

				operands.op1 = pcb_id;
				operands.op2 = pcb_size;

				void *stream = operandos_to_stream(&operands);

				conexion_enviar_stream(memory, MEMORY_INIT, stream, sizeof(operands_t));
				free(stream);

				ssize_t bytes_received = -1;

				uint32_t *page_ref = conexion_recibir_stream(kernel->conexion_memory.socket, &bytes_received);

				if (bytes_received <= 0 && page_ref == NULL)
				{
					LOG_ERROR("[LTS] :=> Couldn't receive page table");
				}
				else
				{
					pcb->page_table = *page_ref;
					LOG_DEBUG("[LTS] :=> Page table  <%d> received", pcb->page_table);
				}
				free(page_ref);
			}
			else
			{
				LOG_WARNING("[LTS] :=> Memory is not connected");
			}
		}

		if (pcb != NULL)
		{
			pcb->status = PCB_READY;

			check_interruption(kernel, pcb);

			safe_queue_push(ready, pcb);

			LOG_INFO("[LTS] :=> PCB #%d  moved to Ready Queue", pcb->id);
		}
		else
		{
			LOG_ERROR("[LTS] :=> No process to be admit - PCB cannot be NULL");
		}

		SIGNAL(kernel->scheduler.execute);
	}
	else
	{
		LOG_ERROR("[LTS] :=> Error while admitting a new process: NULL Queues");
	}
}
