# Enunciado y respuesta

    Un equipo de investigadores necesita hacer un análisis de 7.2TB de datos, donde 1TB=10^12B. Dado que el análisis tomará mucho tiempo, se ha decidido usar un laboratorio de 28 computadoras que están conectadas por un switch de Ethernet con un ancho de banda de 100Mbps y una latencia promedio de 20ms. Restando el ancho de banda extra para "enrutar" y corregir errores (overhead), suponga que la red puede transmitir datos a 85Mbps. Los datos están alojados en una de las 28 computadoras de la red y usted debe tratar de distribuir cantidades iguales de datos a cada una de las otras computadoras del laboratorio. Los datos están en archivos de 1MB cada uno. Los investigadores quieren saber ¿cuánto tiempo tardará en estar listos los datos para iniciar el análisis en el laboratorio?
    
    R/ Los datos tardaran 7,6 dias para estar listos.
    
    **Procedimiento:**
        Se tiene 7.2 TB de datos que se desea enviar a 27 computadoras y mantener 1/28 de informacion en la misma computadora, por lo que se debe enviar:
            > 7.2TB x 27/28 = 6.942 TB
        Al tener que la informacion debe ser enviada en paquetes de 1MB tenemos:
            > 6.942 TB x 10^6 MB/TB = 6.942 x 10^6 MB
        Entonces el tiempo de transmision del mensaje total seria de:
            > T =  20ms + ((6.942 x 10^6 MB)/(85 Mbps x (1/8)MB / Mb))= 653364,725882353 s
        En dias seria:
            > 653365 / 60 / 60 / 24 = 7,6 dias
