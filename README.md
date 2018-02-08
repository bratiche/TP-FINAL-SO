# TPE FINAL SO
## Materiales

* Archivo de construcción: `CMakeLists.txt`, ubicado en el directorio raíz.
* Informe: `docs/Informe.pdf`.
* Códigos fuente: carpetas `client`, `server` y `database`.

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

* server:
* client:
* database:

## Ejecución
### server
```
./server <port>
```
### client
```
./client <hostname> <port>
```
### database
```
./database
```

## Logs

Todos los binarios dejan logs en el sistema, para verlos correr:

```
tail -f /var/log/syslog 
```