
main:
	shared thread_count := read_integer()
	create_thread(secondary, thread_count)
	//How many threads have arrived to the barrier
	shared count := 0
	// Protects the increment of the count
	shared mutex := sempahore(1)
	// Locked (0) until all threads arrive
	shared turnstile1 := semaphore(0)
	shared turnstile2 := semaphore(1)
	
secondary:
	Sentence A
	wait(mutex)
	if ++count == thread_count then
		wait(turnstile2)
		signal(turnstile1)	
	signal(mutex)
	
	wait(turnstile1)
	signal(turnstile1)
	...
	Sentence B

	wait(mutex)
	if --count == 0 then
		wait(turnstile1)
		signal(turnstile2)
	signal(mutex)
	
	wait(turnstile2)
	signal(turnstile2)
