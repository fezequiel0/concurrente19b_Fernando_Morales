# prime_hybrid_int vs prime_process

Al realizar 5 ejecucionces de los 3 programas, se obtuvieron los siguientes datos:

|   Programa	         |	Procesos o hilos 		                    | 		Tiempo			     |
|   ------------         |     ------------------------------------     | 	   -----------		     |
| prime_hybrid_int       |	32 hilos (4 procesos con 8 hilos cada uno)	|      0.095217883		     |
| prime_process          |	32 procesos                                 |	   0.029542216			 |
| prime_process_reduction|  32 procesos                                 |      0.087813377s          |
