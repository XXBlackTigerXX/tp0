#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	int socket_servidor;
	int err = 0;
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(NULL, PUERTO, &hints, &servinfo);
	if (err != 0)
	{
		printf("Error: fallo getaddrinfo servidor");
		abort();
	};
	

	// Creamos el socket de escucha del servidor
	err = socket_servidor = socket(hints.ai_family,hints.ai_socktype,hints.ai_flags);
	if (err == -1)
	{
		printf("Error: fallo de creacion socket de servidor.");
		abort();
	};
	
	// Asociamos el socket a un puerto
	err = bind(fd_socket_escucha_servidor, servinfo.ai_addr, servinfo.ai_addrlen);
	if(err == -1){
		printf("Error: bind socket servidor");
		abort();
	};
	// Escuchamos las conexiones entrantes

	err = listen(socket_servidor, SOMAXCONN);
	if(err == -1) {
		printf("Error: Poner en escucha socket servidor.");
	};


	err = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	if(err == -1) {
		printf("Error: SO_REUSEPORT servidor.");
	};

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");
	abort();

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	// Aceptamos un nuevo cliente
	int socket_cliente;
	err = socket_cliente = accept(socket_servidor, NULL, NULL);

	if(err == -1) {
		printf("Error: SO_REUSEPORT servidor.");
		abort();
	};

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
