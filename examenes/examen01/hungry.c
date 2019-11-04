/*  
    main: 
        shared rest_count := read.integer()
        shared rest_capacityp[rest_count] := 0[rest_count]
        shared rest_current_capacity[rest_count] := 0[rest_count]
        shared capcity_mutex[rest_count]
        shared rest_semaphore[rest_count]
        for local index := 0 to rest_count do   
            rest_capacity[index] := read_integer()
            rest_semaphore[index] := semaphore(rest_capacity[index])
            capacity_mutex[index] := semaphore(1)

        local id := 0
        while true do
            case read_char() of 
                P : create_thread(patient(id))
                I : create_thread(impatient(id))
                EOF: return
                    
    patient(id):
        wait(capacity_mutex[id])
        rest_curent_capacity[id] := ++rest_current_capacity[id]
        signal(capacity_mutex[id])
        wait(rest_semaphores[id])
        eat()
        signal(rest_semaphores[id])
        wait(capacity_mutex[id])
        rest_curent_capacity[id] := --rest_current_capacity[id]
        signal(capacity_mutex[id])
        

    impatient(id)
        rest_differences := 0[rest_count]
        while tries < rest_count and didnt_eat = 1 do
            wait(capacity_mutex[id])
            rest_differences[id] := rest_current_capacity[id] - rest_capacity[id]
            if differences[id] < 0 do
                patient(id)
                didnt_eat := 0
            else do
                ++id mod rest_count
                ++tries
                walk()
            }
        }
        if didnt_eat == 0 do
            lowest_queue := differences[0]
            destination_rest := 0
            for rest_id := 0 to rest_count do
                if differences[rest_id] < lowest_queue do   
                    lowest_queue := differences[rest_id]
                    destination_rest := rest_id
                }
            }
            walk()
            patient(destination_rest)    
        }
    }
    
    
    
    
*/