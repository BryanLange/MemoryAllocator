# OperatingSystems-Allocator  

About: This program manages a contiguous region of memory.  
   		 Memory size specified at execution.  
Commands: RQ P1 200600 B (Request ProcessName Size Approach)  
   		    RL P0 (Release ProcessName)  
   		    C (Compaction)  
   		  	STAT (Status Report)  
   		  	QUIT  
Compile: $ gcc allocator.c -o allocator  
Execute: $ ./allocator 1048576  
