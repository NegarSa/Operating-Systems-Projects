#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(){
    char ip[16] = "127.0.0.1";
    int fd;
    FILE* fp = fopen("netLKM.config", "r");
    fd = open("/dev/netchar", O_RDWR);
    if (fd < 0){
       perror("Failed to open the device...");
       return errno;
    }
    fscanf(fp, "%s", ip);
    int ret = write(fd, ip, strlen(ip) + 1);
    if (ret < 0){
       perror("Failed to write the message to the device.");
       return errno;
    }
    char d[10];
    ret = read(fd, d, 10);
    if (ret < 0){
       perror("Failed to read the message from the device.");
       return errno;
    }
    printf("The received message is: [%s]\n", d);
    while(!feof(fp)){
      fscanf(fp, "%s", ip);
      int ret = write(fd, ip, strlen(ip) + 1);
      if (ret < 0){
         perror("Failed to write the message to the device.");
         return errno;
      }
      char d[10];
      ret = read(fd, d, 10);
      if (ret < 0){
         perror("Failed to read the message from the device.");
         return errno;
      }
      printf("The received message is: [%s]\n", d);
    }

    return 0;
}
