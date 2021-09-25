#ifndef CONFIG_H
#define CONFIG_H

char media[] = "demon-mask-pixel-720p-18fps.mp4";

// compositor[0] = command
// compositor[@] = args
// the last argument has to be NULL
// char* compositor[] = {"/usr/bin/picom", "/home/user/.config/picom/picom.conf", NULL};
char* compositor[] = {NULL};

#endif
