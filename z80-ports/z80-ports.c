#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define Z80_PORTS ".Z80_ports"
#define NUM_PORTS 256

#define dc(c) (isprint(c) ? c : '.')

typedef enum {
  cmdNONE,
  cmdIN,
  cmdOUT,
  cmdDUMP,
  cmdQUIT
} CommandType;

typedef struct {
  CommandType type;
  int port;
  int value;
} Command;

int hex2byte(const char *s) {
  uint8_t b = 0;
  while (*s) {
    char c = *(s++);
    if (c >= '0' && c <= '9') {
      b = (b<<4) | (c - '0');
    }
    else if (c >= 'A' && c <= 'F') {
      b = (b<<4) | (c - 'A' + 10);
    }
    else if (c >= 'a' && c <= 'f') {
      b = (b<<4) | (c - 'a' + 10);
    }
    else {
      return -1;
    }
  }

  return b;
}

Command *get_command() {
  static Command command;
  char buf [80];
  while (1) {
    fputs("> ", stdout);
    fflush(stdout);

    if (fgets(buf, sizeof(buf), stdin) == NULL) return NULL;

    command.type = cmdNONE;
    char *p = strtok(buf, " \t\n");
    size_t plen = strlen(p);
    if (p == NULL) continue;
    if (strncasecmp(p, "IN", plen) == 0
        || strncasecmp(p, "I", plen) == 0) {
      command.type = cmdIN;
    }
    else if (strncasecmp(p, "OUT", plen) == 0
             || strncasecmp(p, "O", plen) == 0) {
      command.type = cmdOUT;
    }
    else if (strncasecmp(p, "DUMP", plen) == 0
             || strncasecmp(p, "D", plen) == 0) {
      command.type = cmdDUMP;
      return &command;
    }
    else if (strncasecmp(p, "QUIT", plen) == 0
             || strncasecmp(p, "Q", plen) == 0) {
      command.type = cmdQUIT;
      return &command;
    }
    else {
      fputs("unrecognized command\n", stdout);
      continue;
    }

    p = strtok(NULL, "\n,");
    if (p == NULL) {
      fputs("port expected\n", stdout);
      continue;
    }

    command.port = hex2byte(p);
    if (command.port == -1) {
      fputs("invalid port\n", stdout);
      continue;
    }

    p = strtok(NULL, "\n");
    if (p == NULL && command.type == cmdOUT) {
      fputs("value expected\n", stdout);
      continue;
    }
    if (p != NULL && command.type == cmdIN) {
      fputs("value not allowed\n", stdout);
      continue;
    }

    if (command.type == cmdOUT) {
      command.value = hex2byte(p);
      if (command.value == -1) {
        fputs("invalid value\n", stdout);
        continue;
      }
    }

    return &command;
  }

  return NULL;
}

void dump_ports(uint8_t* p) {
  for (int i = 0; i < 16; i++) {
    fprintf(stdout, "%02x: %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x  %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c\n", 16*i,
      p[16*i+0], p[16*i+1], p[16*i+2], p[16*i+3],
      p[16*i+4], p[16*i+5], p[16*i+6], p[16*i+7],
      p[16*i+8], p[16*i+9], p[16*i+10], p[16*i+11],
      p[16*i+12], p[16*i+13], p[16*i+14], p[16*i+15],
      dc(p[16*i+0]), dc(p[16*i+1]), dc(p[16*i+2]), dc(p[16*i+3]),
      dc(p[16*i+4]), dc(p[16*i+5]), dc(p[16*i+6]), dc(p[16*i+7]),
      dc(p[16*i+8]), dc(p[16*i+9]), dc(p[16*i+10]), dc(p[16*i+11]),
      dc(p[16*i+12]), dc(p[16*i+13]), dc(p[16*i+14]), dc(p[16*i+15]));
  }
}

int main() {
  int fd = open(Z80_PORTS, O_CREAT | O_TRUNC | O_RDWR);
  if (fd == -1) {
    perror("open");
    exit(1);
  }

  uint8_t buf[NUM_PORTS];
  memset(buf, 0, sizeof(buf));

  int n = write(fd, buf, sizeof(buf));
  if (n < sizeof(buf)) {
    fputs("short write\n", stderr);
    exit(1);
  }

  uint8_t* ports = mmap(NULL, sizeof(NUM_PORTS), PROT_READ | PROT_WRITE,
      MAP_SHARED, fd, 0);
  if (ports == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  int done = 0;
  do {
    Command *command = get_command();
    if (command == NULL) break;
    uint8_t value;
    switch (command->type) {
      case cmdIN:
        value = ports[command->port];
        fprintf(stdout, "IN %02x  <= %02x\n", command->port, value);
        break;
      case cmdOUT:
        ports[command->port] = command->value;
        fprintf(stdout, "OUT %02x => %02x\n", command->port, command->value);
        break;
      case cmdDUMP:
	dump_ports(ports);
        break;
      case cmdQUIT:
        done = 1;
        break;
      default:
	break;
    }
    msync(ports, sizeof(NUM_PORTS), MS_SYNC | MS_INVALIDATE);
  } while (!done);
}
