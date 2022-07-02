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
#include "time.h"
#include "sts.h"
#include "kernel.h"
#include "pcb_unit.h"
#include "cpu_controller.h"
#include "log.h"

void execute(kernel_t *kernel, pcb_t *pcb);
void terminate(kernel_t *kernel, pcb_t *pcb);
void pre_empt(scheduler_t *scheduler, pcb_t *pcb);

void *
short_term_schedule(void *data)
{
	LOG_TRACE("[STS] :=> Short Term Scheduling Running...");

	kernel_t *kernel = (kernel_t *)data;
	scheduler_t sched = kernel->scheduler;

	for (;;)
	{
		pcb_t *pcb = sched.get_next(&sched);

		if (pcb)
			execute(kernel, pcb);
		else
		{
			sleep(10);
		}
	}

	return NULL;
}

void execute(kernel_t *kernel, pcb_t *pcb)
{
	// Update Status to Executing
	pcb->status = PCB_EXECUTING;
	// Time the CPU Usage
	struct timeval start, stop;
	uint32_t real_usage = 0;

	gettimeofday(&start, NULL);

	// Send PCB to CPU so it can execute it
	ssize_t bytes_sent = -1;
	bytes_sent = cpu_controller_send_pcb(kernel->conexion_dispatch, PCB, pcb);

	if (bytes_sent > 0)
	{
		LOG_INFO("[STS] :=> Executing PCB #%d", pcb->id);
		LOG_WARNING("[STS] :=> PCB #%d estimated to use CPU for %dms", pcb->id, pcb->estimation);
		pcb_destroy(pcb);
		pcb = NULL;
	}
	else
	{
		// Fail fast - Terminate immediately
		LOG_ERROR("[STS] :=> PCB could not be sent - Terminated");
		terminate(kernel, pcb);
		return;
	}

	// WAIT for PCB to leave CPU - Include IO time
	uint32_t io_time = 0;
	pcb = (pcb_t *)cpu_controller_receive_pcb(kernel->conexion_dispatch, &io_time);

	// Time real CPU usage
	gettimeofday(&stop, NULL);
	real_usage = time_diff_ms(start, stop);
	pcb->real = real_usage;
	LOG_TRACE("[STS] :=> PCB #%d returned after %dms", pcb->id, pcb->real);

	if (io_time > 0)
	{
		LOG_WARNING("[STS] :=> IO (%d)", io_time);
	}

	// According to PCB Status take different options
	switch (pcb->status)
	{
		// This means PCB went for I/O burst
	case PCB_BLOCKED:
		block(&kernel->scheduler, pcb, io_time);
		LOG_WARNING("[STS] :=> PCB #%d is blocked", pcb->id);
		break;

		// PCB has executed operation EXIT
	case PCB_TERMINATED:
		terminate(kernel, pcb);
		LOG_DEBUG("[STS] :=> PCB #%d has exited", pcb->id);
		break;

		// PCB was preempted.
	case PCB_READY:
		pre_empt(&kernel->scheduler, pcb);
		LOG_TRACE("[STS] :=> PCB #%d was preempted", pcb->id);
		break;

		// Invalid status - terminate program immediately.
	default:
		terminate(kernel, pcb);
		LOG_ERROR("[STS] :=> PCB #%d has a corrupted status (%d). Terminated.", pcb->id, pcb->status);
		break;
	}
}

void terminate(kernel_t *kernel, pcb_t *pcb)
{
	SIGNAL(kernel->scheduler.dom);
	pcb_destroy(pcb);
	pcb = NULL;
}

void block(scheduler_t *scheduler, pcb_t *pcb, uint32_t io_time)
{
	pcb->io = io_time;
	safe_queue_push(scheduler->blocked, pcb);
	SIGNAL(scheduler->io_request);
}

void pre_empt(scheduler_t *scheduler, pcb_t *pcb)
{
	safe_queue_push(scheduler->ready, pcb);
};
