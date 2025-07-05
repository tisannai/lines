#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <plinth.h>


/*
  lines - select lines from a file or stdin.

  Lines are either passed or rejected. User can move forward in lines
  and set pass mode on or off.

  */

const char* lines_version = "0.0.1";


pl_enum( ctype ){ CT_NONE, CT_TOGGLE, CT_PASS, CT_REJECT, CT_MOVE, CT_JUMP, CT_FIND, CT_SKIP };


pl_struct( cmd )
{
    ctype_t type; /**< Command type. */
    union
    {
        int   jump; /**< Jump count. */
        char* tag;  /**< Search tag. */
    };
};


pl_struct( buf )
{
    FILE* fh;    /**< File handle. */
    char* line;  /**< Current line. */
    int   len;   /**< Current line length. */
    int   size;  /**< Line capacity. */
    int   count; /**< Line count. */
};


pl_struct( lstate )
{
    int  rel;    /**< Relative jump count. */
    int  ci;     /**< Command index. */
    char pass;   /**< Pass state. */
    char search; /**< Search state. */
};


pl_struct( sstate )
{
    int len; /**< Search tag length. */
};



void fail_with_msg( char* format, ... )
{
    va_list ap;
    va_start( ap, format );
    vfprintf( stderr, format, ap );
    fputc( '\n', stderr );
    va_end( ap );
    exit( EXIT_FAILURE );
}


int to_unsigned( char* str )
{
    char* end;
    long  res;
    res = strtol( str, &end, 10 );
    if ( ( res < 0 ) || ( str + strlen( str ) != end ) ) {
        fail_with_msg( "Invalid jump value: \"%s\" ...", str );
        return 0;
    } else {
        return (int)res;
    }
}


int get_ln( buf_t buf )
{
    int ch;

    buf->len = 0;
    ch = fgetc( buf->fh );

    while ( ch != EOF ) {
        if ( buf->len >= buf->size ) {
            buf->size = buf->size * 2;
            buf->line = realloc( buf->line, buf->size );
        }
        buf->line[ buf->len ] = ch;
        buf->len++;
        if ( ch == '\n' ) {
            buf->count++;
            break;
        }
        ch = fgetc( buf->fh );
    }

    return 0;
}


void lstate_init( lstate_t l )
{
    l->rel = 0;
    l->ci = 0;
    l->pass = 0;
    l->search = 0;
}


void lout( lstate_t l, buf_t buf )
{
    if ( l->pass ) {
        fwrite( buf->line, 1, buf->len, stdout );
    }
    buf->len = 0;
}


/**
 * Select lines and output if pass state is on.
 */
void select_lines( buf_t buf, cmd_t cmds, int cmd_cnt )
{
    cmd_s cmd;

    lstate_s l;
    sstate_s s;

    lstate_init( &l );
    get_ln( buf );

    while ( 1 ) {

        if ( buf->len == 0 ) {

            lout( &l, buf );
            break;

        } else {

            if ( l.ci >= cmd_cnt ) {

                lout( &l, buf );
                get_ln( buf );

            } else {

                cmd = cmds[ l.ci ];

                switch ( cmd.type ) {

                    case CT_TOGGLE: {
                        l.pass = !l.pass;
                        l.ci++;
                        break;
                    }

                    case CT_PASS: {
                        l.pass = 1;
                        l.ci++;
                        break;
                    }

                    case CT_REJECT: {
                        l.pass = 0;
                        l.ci++;
                        break;
                    }

                    case CT_MOVE: {
                        if ( buf->count < cmd.jump ) {
                            lout( &l, buf );
                            get_ln( buf );
                        } else {
                            l.ci++;
                        }
                        break;
                    }

                    case CT_JUMP: {
                        if ( l.rel < cmd.jump ) {
                            lout( &l, buf );
                            l.rel++;
                            get_ln( buf );
                        } else {
                            l.rel = 0;
                            l.ci++;
                        }
                        break;
                    }

                    case CT_FIND:
                    case CT_SKIP: {

                        if ( l.search == 0 ) {
                            l.search = 1;
                            s.len = strlen( cmd.tag );
                        }

                        if ( buf->len >= s.len && !strncmp( cmd.tag, buf->line, s.len ) ) {
                            l.ci++;
                            l.search = 0;
                            if ( cmd.type == CT_SKIP ) {
                                lout( &l, buf );
                                get_ln( buf );
                            }
                        } else {
                            lout( &l, buf );
                            get_ln( buf );
                        }

                        break;
                    }

                    case CT_NONE: {
                        fail_with_msg( "Internal error: CT_NONE\n" );
                        break;
                    }
                }
            }
        }
    }
}


/**
 * Display command line usage to screen.
 */
void usage( void )
{
    printf( "\n" );
    printf( "  lines <commands> [<file>]\n" );
    printf( "\n" );
    printf( "  -h               Help.\n" );
    printf( "  -d               Documentation.\n" );
    printf( "\n" );
    printf( "  Commands:\n" );
    printf( "    t                Toggle pass state (default: off).\n" );
    printf( "    p                Set pass on.\n" );
    printf( "    r                Set pass off.\n" );
    printf( "    m <index>        Move to line index (first line is at 0).\n" );
    printf( "    r <index>        Jump relative to current line index.\n" );
    printf( "    f <tag>          Find line with tag and stop at match.\n" );
    printf( "    s <tag>          Find line with tag and skip matched line.\n" );
    printf( "    <file>           File (default: stdin).\n" );
    printf( "\n" );
    printf( "  Copyright (c) 2025 by Tero Isannainen\n" );
    printf( "\n" );
    exit( EXIT_FAILURE );
}


void documentation( void )
{
    printf( "`lines` is a small c-program for selecting lines from input file. The\n"
            "input file is a disk file or stdin. The selected lines go to stdout.\n"
            "\n"
            "The selection is performed by progressing through each line, using\n"
            "commands, and selecting which lines are passed and which are rejected.\n"
            "For example:\n"
            "\n"
            "    lines m 10 p\n"
            "\n"
            "will move (`m`, absolute) to line at index 10 and pass (`p`, pass) the\n"
            "following lines until the end of file. The pass state is off, by\n"
            "default, in the beginning of file. The first 10 lines of the file\n"
            "would be displayed with:\n"
            "\n"
            "    lines p j 10 r\n"
            "\n"
            "`p` (pass) sets pass state on. `j` jumps (relative from current, i.e.\n"
            "from 0 to 10) to index 10. `r` (reject) sets the pass state off. Line\n"
            "indeces start from 0.\n"
            "\n"
            "In addition to absolute and relative indeces, the line position can be\n"
            "searched. With `f`/`s` command, the given tag is searched from the\n"
            "beginning of line. If tag is found, the current line index is set to\n"
            "the matched line, otherwise the end of file is reached.\n"
            "\n"
            "Commands:\n"
            "\n"
            "* `t`, toggle: Toggle output.\n"
            "\n"
            "* `p`, pass: Enable output.\n"
            "\n"
            "* `r`, reject: Disable output.\n"
            "\n"
            "* `m <value>`, absolute: Move to line index.\n"
            "\n"
            "* `j <value>`, relative: Jump relative to current line index.\n"
            "\n"
            "* `f <tag>`, find, no skip: Search string and stop to matching line.\n"
            "\n"
            "* `s <tag>`, find, with skip: Search string and jump over the matching\n"
            "  line.\n" );

    exit( EXIT_FAILURE );
}


int main( int argc, char* argv[] )
{
    int oi;

    cmd_t cmds;
    int   ci;

    char* file;

    buf_s buf;


    oi = 1;

    cmds = malloc( argc * sizeof( cmd_s ) );
    ci = 0;

    file = NULL;

    while ( oi < argc ) {
        if ( 0 ) {
        } else if ( !strcmp( argv[ oi ], "-h" ) ) {
            usage();
        } else if ( !strcmp( argv[ oi ], "-d" ) ) {
            documentation();
        } else if ( !strcmp( argv[ oi ], "t" ) ) {
            cmds[ ci++ ].type = CT_TOGGLE;
        } else if ( !strcmp( argv[ oi ], "p" ) ) {
            cmds[ ci++ ].type = CT_PASS;
        } else if ( !strcmp( argv[ oi ], "r" ) ) {
            cmds[ ci++ ].type = CT_REJECT;
        } else if ( !strcmp( argv[ oi ], "m" ) ) {
            oi++;
            cmds[ ci ].type = CT_MOVE;
            cmds[ ci++ ].jump = to_unsigned( argv[ oi ] );
        } else if ( !strcmp( argv[ oi ], "j" ) ) {
            oi++;
            cmds[ ci ].type = CT_JUMP;
            cmds[ ci++ ].jump = to_unsigned( argv[ oi ] );
        } else if ( !strcmp( argv[ oi ], "f" ) ) {
            oi++;
            cmds[ ci ].type = CT_FIND;
            cmds[ ci++ ].tag = argv[ oi ];
        } else if ( !strcmp( argv[ oi ], "s" ) ) {
            oi++;
            cmds[ ci ].type = CT_SKIP;
            cmds[ ci++ ].tag = argv[ oi ];
        } else {
            file = argv[ oi ];
        }
        oi++;
    }

    if ( file ) {
        buf.fh = fopen( file, "r" );
        if ( buf.fh == NULL ) {
            fail_with_msg( "Could not open file: \"%s\" ...", file );
        }
    } else {
        buf.fh = stdin;
    }

    buf.size = 8192;
    buf.line = malloc( buf.size );
    buf.count = -1;

    select_lines( &buf, cmds, ci );

    if ( buf.fh != stdin ) {
        fclose( buf.fh );
    }

    free( buf.line );
    free( cmds );

    exit( EXIT_SUCCESS );
}
