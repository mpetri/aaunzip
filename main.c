/*
 * File:   main.c
 * Author: matt
 *
 * Created on 28 November 2010, 3:22 PM
 */

#include "libutil.h"
#include "libhuff.h"
#include "liblupdate.h"
#include "libbwt.h"

enum mode_t {
    UNKNOWN,
    SIMPLE,
    MTF,
    FC,
    WFC,
    TS
};

static void
print_usage(const char* program)
{
    fprintf(stderr, "USAGE: %s <input>\n", program);
    fprintf(stderr, "  -h Display usage information\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "EXAMPLE: %s test.dat.aazip\n",
            program);
    fprintf(stderr, "\n");
    return;
}

/*
 *
 */
int main(int argc, char** argv)
{
    bit_file_t* f;
    FILE* outf;
    char* infile,*outfile;
    uint8_t* input,*bwt,*output,lumode;
    int32_t I,n;
    mode_t lupdate_alg;

    /* parse command line parameter */
    if (argc !=2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    infile = argv[1];

    /* read input file */
    f = BitFileOpen(infile, BF_READ);

    if (!f) {
        fatal("could not open file %s.",infile);
    }

    /* check if compressed with aazip */
    if (BitFileGetChar(f) != 'A' || BitFileGetChar(f) != 'A') {
        fatal("file %s not compressed with aazip.",infile);
    }

    fprintf(stdout,"FILE: %s\n",infile);

    /* read header */
    BitFileGetBitsInt(f,&I,32,sizeof(I));
    BitFileGetBitsInt(f,&lumode,8,sizeof(lumode));
    lupdate_alg = lumode;

    input = decode_huffman(f,&n);

    /* malloc output memory */
    bwt = safe_malloc(n*sizeof(uint8_t));

    /* peform list update */
    switch (lupdate_alg) {
        case SIMPLE:
            fprintf(stdout,"LUPDATE: Simple\n");
            bwt = lupdate_simple(input,n,bwt);
            break;
        case MTF:
            fprintf(stdout,"LUPDATE: Move-To-Front\n");
            bwt = lupdate_movetofront(input,n,bwt);
            break;
        case FC:
            fprintf(stdout,"LUPDATE: FC\n");
            bwt = lupdate_freqcount(input,n,bwt);
            break;
        case WFC:
            fprintf(stdout,"LUPDATE: WFC\n");
            bwt = lupdate_wfc(input,n,bwt);
            break;
        case TS:
            fprintf(stdout,"LUPDATE: TS\n");
            bwt = lupdate_timestamp(input,n,bwt);
            break;
        default:
            fatal("unkown list update algorithm.");
    }

    /* reverse bwt */
    output = reverse_bwt(bwt,n,I,input);

    /* write output */
    outfile = safe_strcat(infile,".org");
    outf = safe_fopen(outfile,"w");
    if (fwrite(output,sizeof(uint8_t),n,outf)!= (size_t)n) {
        fatal("error writing output.");
    }
    safe_fclose(outf);


    /* clean up */
    free(bwt);
    free(input);
    BitFileClose(f);

    return (EXIT_SUCCESS);
}

