#include "conexion_dispatch.h"
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

int on_connect_dispatch(void *conexion, bool offline_mode)
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
	char *port = puerto_cpu_dispatch();
	char *ip = ip_cpu();

	LOG_DEBUG("Connecting <Cpu> at %s:%s", ip, port);
	// Test connection with cpu
	kernel->conexion_dispatch = conexion_cliente_create(ip, port);

	if (on_connect_dispatch(&kernel->conexion_dispatch, false) EQ SUCCESS)
	{
		// Test connection with cpu
		LOG_DEBUG("Connected as CLIENT at %s:%s", ip, port);
	}

	return SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

void *routine_conexion_dispatch(void *data)
{
	kernel_t *kernel = data;

	conexion_init(kernel);

	conexion_enviar_mensaje(kernel->conexion_dispatch, "Mando un msj");

	for (;;)
	{
		LOG_WARNING("Hola Thread DISPATCH");
		sleep(TIEMPO_ESPERA);
	}
}