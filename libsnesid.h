// Copyright (c) 2025 Tara Keeling
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _LIBSNESID_H_
#define _LIBSNESID_H_

// SNES Button bit definitions
// Source: https://gamefaqs.gamespot.com/snes/916396-super-nintendo/faqs/5395

#define SNES_Button_B 0x0001
#define SNES_Button_Y 0x0002
#define SNES_Button_Select 0x0004
#define SNES_Button_Start 0x0008
#define SNES_Button_Up 0x0010
#define SNES_Button_Down 0x0020
#define SNES_Button_Left 0x0040
#define SNES_Button_Right 0x0080
#define SNES_Button_A 0x0100
#define SNES_Button_X 0x0200
#define SNES_Button_L 0x0400
#define SNES_Button_R 0x0800

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Connects to a running snesid daemon socket
/// @param socketPath 
/// @return 0 Upon success
int snesdConnect( const char* socketPath );

/// @brief Disconnects from snesid daemon
/// @param  
void snesdDisconnect( void );

/// @brief Polls the attached SNES controller
/// @param  
/// @return 0 Upon success
int snesdPollKeys( void );

/// @brief 
/// @param  
/// @return Returns a field of buttons currently pressed (not held)
int snesdKeysDown( void );

/// @brief 
/// @param  
/// @return Returns a field of buttons currently held down
int snesdKeysHeld( void );

/// @brief 
/// @param  
/// @return Returns a field of buttons that were just released
int snesdKeysUp( void );

#ifdef __cplusplus
}
#endif

#ifdef LIBSNESID_IMPLEMENTATION

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t lastKeyState = 0;
static uint32_t curKeyState = 0;

static uint32_t keysHeld = 0;
static uint32_t keysDown = 0;
static uint32_t keysUp = 0;

static int snesdFd = -1;

int snesdConnect( const char* socketPath ) {
    struct sockaddr_un serverSockAddr;

    if ( ( snesdFd = socket( AF_UNIX, SOCK_STREAM, 0 ) ) != -1 ) {
        memset( &serverSockAddr, 0, sizeof( struct sockaddr_un ) );

        strncpy( serverSockAddr.sun_path, socketPath, strlen( socketPath ) + 1 );
        serverSockAddr.sun_family = AF_UNIX;

        return connect( snesdFd, ( const struct sockaddr* ) &serverSockAddr, sizeof( struct sockaddr_un ) );
    }

    return -1;
}

void snesdDisconnect( void ) {
    if ( snesdFd != -1 ) {
        close( snesdFd );
        snesdFd = -1;
    }

    lastKeyState = 0;
    curKeyState = 0;

    keysHeld = 0;
    keysDown = 0;
    keysUp = 0;
}

int snesdPollKeys( void ) {
    const uint8_t keyReq = 0x20;
    uint8_t buf[ 2 ];

    if ( snesdFd != -1 ) {
        if ( write( snesdFd, &keyReq, 1 ) == 1 ) {
            if ( read( snesdFd, buf, 2 ) == 2 ) {
                curKeyState = ( buf[ 1 ] << 8 ) | buf[ 0 ];

                keysHeld = curKeyState;
                keysDown = curKeyState & ~lastKeyState;
                keysUp = lastKeyState & ~curKeyState;

                lastKeyState = curKeyState;

                return 0;
            }
        }
    }

    return -1;
}

int snesdKeysDown( void ) {
    return keysDown;
}

int snesdKeysHeld( void ) {
    return keysHeld;
}

int snesdKeysUp( void ) {
    return keysUp;
}

#ifdef __cplusplus
}
#endif

#endif

#endif
