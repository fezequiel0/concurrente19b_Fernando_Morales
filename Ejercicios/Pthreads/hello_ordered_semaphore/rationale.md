# Diferencias entre usar mutex y semaphore

En el resultado, el uso tanto de mutex como de semaphore no afectan en ningún sentido el resultado del output y ambas soluciones lo solucionan perfectamente.
Conceptualmente, incluso, funcionan similarmente, donde cada thread va a ser capaz de accesar e incrementar los valores de los mecanismos de sincronización diferentes, por lo que el thread con thread_num = 0 cambiará el valor del mecanismo de sincronización respectivo del thread con thread_num = 1 y así sucesivamente. Ambos mecanismos de sincronización permiten la espera pasiva, por lo que la única diferencia relevante entre ambas soluciones es el espacio que se gasta en cada una de estas.
