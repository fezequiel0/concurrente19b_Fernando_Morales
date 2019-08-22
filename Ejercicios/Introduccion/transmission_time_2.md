# Enunciado y respuesta

    En un instituto sismográfico se tienen 40 sensores que recolectan y acumulan datos constantemente. Cada minuto envían un archivo de 1.5MB hacia los servidores centrales, donde 1MB=10^6B. Los sensores se turnan para enviar los datos con el fin de no saturar los servidores, y puede asumir que no ocurren colisiones de datos en la red entre los 40 sensores. Se tiene contratado un servicio de 16Mbps con el proveedor de servicio de Internet, por tanto, los servidores pueden recibir datos a esa velocidad. Se ha medido una latencia promedio de 2s porque hay sensores muy remotos. Se quieren instalar 30 sensores más ¿Se puede mantener el mismo ancho de banda con el proveedor de servicio o se debe aumentar? En caso de incrementarse ¿cuánto sería el ancho de banda mínimo que asegure que los datos se pueden recibir por los servidores centrales?
    
    R/ Si cada minuto cada sensor envia 1.5 MB de informacion a los servidores centrales, eso significa que cada minuto se envian 1.5MB x 40= 60MB de informacion y si utilizamos la formula de tiempo de transmision de mensaje entonces:
        > T = 2s + 60MB/2MBps = 32 s
    Por lo que originalmente el ancho de banda es suficiente para 40 sensores.
    
    Se desea agregar 30 sensores a la misma red, por lo que la nueva cantidad de datos enviada total por minuto seria 1.5MB x 70 = 105MB de informacion y utilizando la misma formula de tiempo obtenemos:
        > T = 2s + 105MB/2MBps = 54.5 s
    Por lo que no habria ningun problema implementando 30 sensores mas a la red de dispositivos; no habria necesidad alguna de aumentar el ancho de banda.
