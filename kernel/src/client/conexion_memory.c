#include "conexion_memory.h"
#include "kernel.h"
#include "instruction.h"
#include "conexion.h"
#include "accion.h"
#include "log.h"
#include "cfg.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern kernel_t g_kernel;

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static int on_connect(void *conexion, bool offline_mode)
{
	if (offline_mode)
	{
		LOG_WARNING("Module working in offline mode.");
		return ERROR;
	}

	while (!conexion_esta_conectada(*(conexion_t *)conexion))
	{
		LOG_TRACE("Connecting...");

		if (conexion_conectar((conexion_t *)conexion) EQ ERROR)
		{
			LOG_ERROR("Could not connect.");
			sleep(TIEMPO_ESPERA);
		}
	}

	return SUCCESS;
}

static int conexion_init(kernel_t *kernel)
{
	char *port = puerto_memoria();
	char *ip = ip_memoria();

	LOG_DEBUG("[MEMORY-THREAD] - Connecting <Kernel> at %s:%s", ip, port);

	kernel->conexion_memory = conexion_cliente_create(ip, port);

	if (on_connect(&kernel->conexion_memory, false) EQ SUCCESS)
	{
		LOG_DEBUG("Connected as CLIENT at %s:%s", ip, port);
	}

	return SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

// Rutina (HILO) de Kernel como cliente.
void *routine_conexion_memoria(void *data)
{
	kernel_t *kernel = data;

	conexion_init(kernel);

	conexion_enviar_mensaje(kernel->conexion_memory, "Send message");

	for (;;)
	{
		LOG_WARNING("Thread 2 message");
		sleep(TIEMPO_ESPERA);
	}
}
