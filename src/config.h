#ifndef CONFIG_H
#define CONFIG_H

// name of the file that should be used with xwinwrap
// char media[] = "demon-mask-pixel-720p-18fps.mp4"
char media[] = "";

// compositor[0] = command
// compositor[@] = args
// the last argument has to be NULL
// char* compositor[] = {"/usr/bin/picom", "--config", "/home/user/.config/picom/picom.conf", NULL};
char* compositor[] = {NULL};

#endif
