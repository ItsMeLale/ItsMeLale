/*
?       ██╗   ██╗████████╗██╗██╗     ██╗████████╗██╗   ██╗
?       ██║   ██║╚══██╔══╝██║██║     ██║╚══██╔══╝╚██╗ ██╔╝
?       ██║   ██║   ██║   ██║██║     ██║   ██║    ╚████╔╝
?       ██║   ██║   ██║   ██║██║     ██║   ██║     ╚██╔╝
?       ╚██████╔╝   ██║   ██║███████╗██║   ██║      ██║
?        ╚═════╝    ╚═╝   ╚═╝╚══════╝╚═╝   ╚═╝      ╚═╝
*/


//Funzione per trovare lunghezza buffer
unsigned int strlen (char *stringa)
{
    unsigned int i = 1;
    while (*stringa != '\0')
    {
        i++;
        stringa++;
    }
    return i;
}

// A function to copy block of 'n' bytes from source
// address 'src' to destination address 'dest'.
void myMemMove(void *dest, void *src, size_t n)
{
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
  
   // Create a temporary array to hold data of src
   char *temp = new char[n];
  
   // Copy data from csrc[] to temp[]
   for (int i=0; i<n; i++)
       temp[i] = csrc[i];
  
   // Copy data from temp[] to cdest[]
   for (int i=0; i<n; i++)
       cdest[i] = temp[i];
  
   delete [] temp;
}
