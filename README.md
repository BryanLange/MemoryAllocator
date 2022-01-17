# OperatingSystems-Allocator  

<ul>
   <li> About:
      <ul> 
         <li> This program manages a contiguous region of memory. </li>
         <li> Memory size specified at execution. </li> 
      </ul> 
   </li>
   <li> Commands:
      <ul> 
         <li> RQ P1 200600 B (Request ProcessName Size Approach) </li>
   	   <li> RL P0 (Release ProcessName) </li>  
         <li> C (Compaction) </li>  
         <li> STAT (Status Report) </li>  
         <li> QUIT </li>
      </ul>
   </li>
   <li> Compile:
      <ul>
         <li> $ gcc allocator.c -o allocator </li>
      </ul>
   </li>
   <li> Execute: $ ./allocator 1048576 </li>
 </ul>  
Execute:   
