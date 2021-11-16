#ifndef CONFIG_H
#define CONFIG_H

/**
 * time in seconds to delay each call to start xwinwrap and compositor.
 * as the "-ov" flag will draw above any fullscreened window and without it
 * when starting aux_lwallpaper from xinitrc will show only a black screen.
 * it's needed at least 2 seconds between each call to start xwinwrap and a compositor.
*/
unsigned short delay_time = 2;

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
