# prime_hybrid_int vs prime_process

Al realizar 5 ejecucionces de ambos programas, se obtuvieron los siguientes datos:

|   Programa	   |	Procesos o hilos 		                    | 		Tiempo			     |
|   ------------   |     ------------------------------------       | 	   -----------		     |
| prime_hybrid_int |	32 hilos (4 procesos con 8 hilos cada uno)	|      0.095217883		     |
| prime_process    |	32 procesos                                 |	   0.029542216			 |

Por lo que se nota que el mejor tiempo promedio entre ambos programas lo obtuvo prime_process. Esto puede ser ya que, como en prime_hybrid_int se utilizan muchos threads, la comunicación que deben que tener entre sí es superior a la que deben tener dos procesos sin zonas críticas entre sí.   
