#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
  lines - select lines from a file or stdin.

  Select (include) lines range between "left" and "right" limits
  (inclusive). If the "-i" option is specified, the range in inverted
  and becomes an exclusion range.

  */

const char* lines_version = "0.0.1";


/**
 * Select lines and output if selection is enabled.
 *
 * @param fh        Input file handle.
 * @param left      Left (first) limit.
 * @param right     Right (second) limit (or zero for no limit).
 * @param invert    Invert selection logic.
 */
void select_lines( FILE* fh, int left, int right, int invert )
{
    int line_cnt;
    int ch;
    int enable;
    int exit_limit;

    ch = 0;
    line_cnt = 1;

    /*
      -- IN --
      line1
      line2 <- left
      line3
      line4 <- right
      line5
     */

    if ( left == 1 ) {
        enable = 1;
    } else {
        enable = 0;
    }

    if ( right ) {
        exit_limit = ( right + 1 );
    }

    if ( invert ) {
        enable = !enable;
        exit_limit = 0;
    }

    while ( 1 ) {

        ch = fgetc( fh );

        if ( ch == -1 ) {
            break;
        }

        if ( enable ) {
            fputc( ch, stdout );
        }

        if ( ch == '\n' ) {
            line_cnt++;
            if ( left == line_cnt ) {
                enable = !enable;
            }
            if ( ( right + 1 ) == line_cnt ) {
                enable = !enable;
            }
        }

        if ( exit_limit > 0 && line_cnt >= exit_limit ) {
            break;
        }
    }
}


/**
 * Display command line usage to screen.
 */
void usage( void )
{
    printf( "\n" );
    printf( "  lines [l <left>] [r <right>] [i] [<file>]\n" );
    printf( "\n" );
    printf( "  l                Left (first) line limit (default: 1).\n" );
    printf( "  r                Right (last) line limit (default: none).\n" );
    printf( "  i                Invert selection logic.\n" );
    printf( "  <file>           File (default: stdin).\n" );
    printf( "\n" );
    printf( "  Copyright (c) 2025 by Tero Isannainen\n" );
    printf( "\n" );
    exit( EXIT_FAILURE );
}


int main( int argc, char* argv[] )
{
    int   opt_index;
    char* opt;

    int   left;
    int   right;
    int   invert;
    char* file;
    FILE* fh;

    left = -1;
    right = -1;
    invert = 0;
    file = NULL;

    opt_index = 1;
    while ( opt_index < argc ) {
        opt = argv[ opt_index ];
        if ( 0 ) {
        } else if ( !strcmp( opt, "-h" ) ) {
            usage();
            opt_index++;
            opt = argv[ opt_index ];
            left = strtol( opt, NULL, 10 );
        } else if ( !strcmp( opt, "l" ) ) {
            opt_index++;
            opt = argv[ opt_index ];
            left = strtol( opt, NULL, 10 );
        } else if ( !strcmp( opt, "r" ) ) {
            opt_index++;
            opt = argv[ opt_index ];
            right = strtol( opt, NULL, 10 );
        } else if ( !strcmp( opt, "i" ) ) {
            invert = 1;
        } else {
            opt = argv[ opt_index ];
            file = opt;
        }
        opt_index++;
    }

    if ( file ) {
        fh = fopen( file, "r" );
        if ( fh == NULL ) {
            fprintf( stderr, "Could not open file: \"%s\"...\n", file );
            exit( EXIT_FAILURE );
        }
    } else {
        fh = stdin;
    }

    if ( left == -1 ) {
        left = 1;
    }

    if ( right == -1 ) {
        right = 0;
    }

    select_lines( fh, left, right, invert );

    if ( fh != stdin ) {
        fclose( fh );
    }

    exit( EXIT_SUCCESS );
}
