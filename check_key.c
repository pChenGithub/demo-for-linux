#include <stdio.h>
#include <linux/input.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int do_reboot() {
	
	system("am broadcast -a com.smdt.changes.low");
	return 0;
}
 
int main ()  
{  
  int keys_fd;  
  char ret[2];  
  struct input_event t;  
  
  keys_fd = open ("/dev/input/event0", O_RDONLY);  
  if (keys_fd <= 0) {  
      perror("open /dev/input/event2 device error!\n");  
      return -1;  
   }  
  
  while (1)  
    {  
		if (read (keys_fd, &t, sizeof (t)) == sizeof (t)) {  
			if (t.type == EV_KEY)
				if (t.code == 116)  
				{  
					  printf ("key %d %s\n", t.code,  
							  (t.value) ? "Pressed" : "Released");
					do_reboot();
				}
        }  
    }  
  close (keys_fd);  
  
  return 0;  
}  
