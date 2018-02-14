# TPE FINAL SO
## Materiales

* Archivo de construcción: `CMakeLists.txt`, ubicado en el directorio raíz.
* Informe: `docs/Informe.pdf`.
* Códigos fuente: carpetas `src/client`, `src/server` y `src/database`.

## Compilación

Para compilar correr en el directorio raíz:
 
```
cmake .
make
```

### Dependencias

* sqlite3 :  `sudo apt-get install libsqlite3-dev`


### Artefactos generados

Se generan tres binarios en la raíz del directorio con los nombres:

* server: servidor concurrente TCP
* client: cliente que provee una interfaz para interactuar con el servidor
* database: programa externo de consulta y manipulacion de base de datos 

## Ejecución
### server
```
./server <port?>
```
Una vez ejecutado se escucharan pedidos de conexion en el puerto elegido.
### client
```
./client <hostname?> <port?>
```
Luego de establecer la conexion con el servidor se presenta una interfaz para poder realizar consultas a la base de datos.
### database
```
./database
```
Permite manipular la base de datos mediante el protocolo definido en `src/protocol.h`.

## Logs

Todos los binarios dejan logs en el sistema, para verlos correr:

```
tail -f /var/log/syslog 
```

Ejemplo:

```
Feb 14 17:56:14 ubuntu client: [CLIENT] connected to localhost:12345
Feb 14 17:56:14 ubuntu server: [SERVER] [+] socket 4
Feb 14 17:56:31 ubuntu : [DATABASE] request ADD_CLIENT(user)
Feb 14 17:56:31 ubuntu : [DATABASE] response OK
Feb 14 17:56:38 ubuntu server: [SERVER] [-] socket 4
Feb 14 17:56:38 ubuntu client: [CLIENT] disconnected
```