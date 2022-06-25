/**
 * @file sts.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-03-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sts.h"
#include "kernel.h"
#include "pcb_unit.h"
#include "cpu_controller.h"
#include "log.h"

void execute(kernel_t *kernel, pcb_t *pcb);
void terminate(kernel_t *kernel, pcb_t *pcb);

void *short_term_schedule(void *data)
{
	LOG_TRACE("[STS] :=> Initializing Short Term Scheduler");

	kernel_t *kernel = (kernel_t *)data;
	scheduler_t sched = kernel->scheduler;

	for (;;)
	{
		LOG_INFO("[STS] :=> Entering Short Term Scheduler");
		pcb_t *pcb = sched.get_next(&sched);

		if (pcb)
			execute(kernel, pcb);
		else
		{
			LOG_ERROR("[STS] :=> No PCB to execute");
			sleep(10);
		}
	}
}

void execute(kernel_t *kernel, pcb_t *pcb)
{
	pcb->status = PCB_EXECUTING;
	ssize_t bytes_sent = -1;

	bytes_sent = cpu_controller_send_pcb(kernel->conexion_dispatch, PCB, pcb);

	if (bytes_sent > 0)
	{
		LOG_INFO("Executing PCB(id=%d)", pcb->id);
	}
	else
	{
		LOG_ERROR("PCB could not be sent");
	}

	pcb_destroy(pcb);
	pcb = NULL;
	pcb = (pcb_t *)cpu_controller_receive_pcb(kernel->conexion_dispatch);

	switch (pcb->status)
	{
	case PCB_BLOCKED:
		break;

	case PCB_TERMINATED:
		terminate(kernel, pcb);
		break;

	default:
		safe_queue_push(kernel->scheduler.ready, pcb);
		break;
	}
}

void terminate(kernel_t *kernel, pcb_t *pcb)
{
	SIGNAL(kernel->scheduler.dom);
	pcb_destroy(pcb);
	pcb = NULL;
}