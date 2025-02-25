#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


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
    printf( "  lines -l <left> [-r <right>] [-i] *default*\n" );
    printf( "\n" );
    printf( "  -l               Left (first) line limit.\n" );
    printf( "  -r               Right (last) line limit (default: none).\n" );
    printf( "  -i               Invert selection logic.\n" );
    printf( "  *default*        File (default: stdin).\n" );
    printf( "\n" );
    printf( "  Copyright (c) 2025 by Tero Isannainen\n" );
    printf( "\n" );
    exit( EXIT_FAILURE );
}


int main( int argc, char* argv[] )
{
    int option;
    int option_index;

    int   left;
    int   right;
    int   invert;
    char* file;
    FILE* fh;


    /* clang-format off */
    static struct option long_options[] = {
        { "help", no_argument, 0, 'h' },
        { "left", required_argument, 0, 'l' },
        { "right", required_argument, 0, 'r' },
        { "invert", no_argument, 0, 'i' },
        { "file", required_argument, 0, 0 },
        { 0, 0, 0, 0 }
    };
    /* clang-format on */

    left = -1;
    right = -1;
    invert = 0;

    option_index = 0;

    /* Parse command line options. */
    while ( 1 ) {

        option = getopt_long( argc, argv, "l:r:ih", long_options, &option_index );

        if ( option == -1 ) {
            break;
        }

        switch ( option ) {
            case 'h': // --help or -h
                usage();
                break;

            case 'l':
                left = strtol( optarg, NULL, 10 );
                break;

            case 'r':
                right = strtol( optarg, NULL, 10 );
                break;

            case 'i':
                invert = 1;
                break;

            default:
                fprintf( stderr, "Use --help to see valid options.\n" );
                exit( EXIT_FAILURE );
                break;
        }
    }

    if ( optind < argc ) {
        file = argv[ optind ];
        fh = fopen( file, "r" );
    } else {
        file = NULL;
        fh = stdin;
    }

    if ( left == -1 ) {
        usage();
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
