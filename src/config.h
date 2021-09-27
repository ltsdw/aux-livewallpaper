#ifndef CONFIG_H
#define CONFIG_H


// name of the file that should be used with xwinwrap
// char media[] = "demon-mask-pixel-720p-18fps.mp4"
char media[] = NULL;

// compositor[0] = command
// compositor[@] = args
// the last argument has to be NULL
// char* compositor[] = {"/usr/bin/picom", "--config", "/home/user/.config/picom/picom.conf", NULL};
char* const compositor[] = { NULL };

// disable_with: an array of the application
// where aux_lwallpaper should disable eye candy
// should match exactly name of the process
// should be NULL terminated
// const char* const disable_with[] = { "minecraft-launc", NULL };
char* const disable_with[] = { NULL };

#endif
