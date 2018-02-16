# TPE FINAL SO
## Materiales

* Archivo de construcción: `build.sh`, ubicado en el directorio raíz.
* Informe: `docs/Informe.pdf`.
* Códigos fuente: carpetas `src` y `tests`.

## Compilación

Para compilar correr en el directorio raíz:
 
```
./build.sh
```

### Dependencias
* cmake (versión 3.5)
* sqlite3 :  `sudo apt-get install libsqlite3-dev`

### Artefactos generados

Se generan tres binarios en el directorio `build` con los nombres:

* server: servidor concurrente TCP
* client: cliente que provee una interfaz para interactuar con el servidor
* database: programa externo de consulta y manipulación de base de datos

Además se generan los binarios correspondientes a los tests en `build/tests` 

## Ejecución
### server
```
./server [options]
```
options:
* -p \<port\> : puerto (`12345` por default)
* -f \<file\> : archivo de base de datos (`cinema.db` por default)

Una vez ejecutado se escucharán pedidos de conexión en el puerto elegido.
### client
```
./client [options]
```
options:
* -h \<host\> : dirección del servidor (`localhost` por default)
* -p \<port\> : puerto (`12345` por default)

Luego de establecer la conexión con el servidor se presenta una interfaz para poder realizar consultas a la base de datos.
### database
```
./database <filename>
```
Permite manipular la base de datos ubicada en el archivo `filename` mediante el protocolo definido en `src/protocol.h`.
### tests
```
cd build/tests
ctest --output-on-failure
```
Corre todos los tests de unidad.
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