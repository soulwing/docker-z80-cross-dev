#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define Z80_PORTS ".Z80_ports"
#define NUM_PORTS 256

#define DELAY 50*1000	
#define BIT_RD  1
#define BIT_WR  2

#define PORT_BASE 0x10
#define PORT_RD_CONSOLE 0
#define PORT_WR_CONSOLE 1
#define PORT_CTRL 2

#define read_port(i) (ports[PORT_BASE + i])
#define write_port(i,v) (ports[PORT_BASE + i] = v)

uint8_t *mmap_ports(const char *filename, uint8_t offset, uint8_t count) {
  int fd = -1;
  uint8_t *ports = MAP_FAILED;
  uint8_t *buf = NULL;

  fd = open(Z80_PORTS, O_CREAT | O_RDWR, 0644);
  if (fd == -1) {
    perror("open");
    goto map_failed;
  }

  if (lseek(fd, offset, SEEK_SET) == -1) {
    perror("lseek");
    goto map_failed;
  }

  buf = malloc(count);
  memset(buf, 0, count);
  int n = write(fd, buf, count);
  if (n == -1) {
    perror("write");
    goto map_failed;
  }
  if (n < count) {
    fprintf(stderr, "write: expected %d but wrote only %d\n", count, n);
    goto map_failed;
  }

  ports = mmap(NULL, NUM_PORTS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ports == MAP_FAILED) {
    perror("mmap");
  }

map_failed:
  if (fd != -1) close(fd);
  if (buf != NULL) free(buf);
  return ports;
}

int main() {
  char c;
  int n;
  uint8_t* ports = mmap_ports(Z80_PORTS, PORT_BASE, 3);
  if (ports == MAP_FAILED) exit(1);

  while (1) {
    uint8_t ctrl = read_port(PORT_CTRL);
    if ((ctrl & BIT_RD) != 0) {
      n = read(STDIN_FILENO, &c, 1);
      if (n == -1) {
        perror("read(STDIN)");
        exit(1); 
      }
      if (n == 0) exit(0);
      write_port(PORT_RD_CONSOLE, c);
    }    
    if ((ctrl & BIT_WR) != 0) {
      c = read_port(PORT_WR_CONSOLE);
      n = write(STDOUT_FILENO, &c, 1);
      while (n == 0) {
        n = write(STDOUT_FILENO, &c, 1);
      }
      if (n == -1) {
        perror("write(STDOUT)");
        exit(1); 
      }
    }
    if (ctrl != 0) {
      write_port(PORT_CTRL, 0);
    }
    else {
      usleep(DELAY);
    }
  }    
}
