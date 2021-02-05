#include "qemu/osdep.h"
#include "hw/pci/msix.h"
#include "qemu/error-report.h"

#include "../nvme.h"
#include "./nand.h"

/* Profiled from Micron L95B MLC NAND chips */

/*
 * Lower/Upper page pairing in one block
 * Shadow page programming sequence to reduce cell-to-cell interference
 */
void init_nand_page_pairing(FemuCtrl *n)
{
    int i;
    int lowp[] = {0, 1, 2, 3, 4, 5, 7, 8, 502, 503, 506, 507, 509, 510};
    int uppp[] = {6, 9, 504, 505, 508, 511};
    int lpflag = MLC_LOWER_PAGE;

    for (i = 0; i < sizeof(lowp)/sizeof(lowp[0]); i++)
        mlc_tbl[lowp[i]] = MLC_LOWER_PAGE;

    for (i = 0; i < sizeof(uppp)/sizeof(uppp[0]); i++)
        mlc_tbl[uppp[i]] = MLC_UPPER_PAGE;

    for (i = 10; i <= MAX_SUPPORTED_PAGES_PER_BLOCK - 12; i += 2) {
        mlc_tbl[i] = mlc_tbl[i+1] = lpflag;
        lpflag = (lpflag == MLC_LOWER_PAGE) ? MLC_UPPER_PAGE : MLC_LOWER_PAGE;
    }
}

// TLC Layout as described in the paper
// Characterization and Error-Correcting Codes for TLC Flash Memories ICNC'2012
// by Yaakobi et. al.

void init_tlc_page_pairing(FemuCtrl *n)
{
    int i, j;
    int rows = (MAX_SUPPORTED_PAGES_PER_BLOCK + 5) / 6;
    int lpflag = TLC_LOWER_PAGE;
    int page_per_row=6;

    int lowp[] = {0, 1, 2, 3, 4, 5};
    int centerp[] = {6, 7};

    for (i = 0; i < sizeof(lowp)/sizeof(lowp[0]); i++)
        tlc_tbl[lowp[i]] = TLC_LOWER_PAGE;

    for (i = 0; i < sizeof(centerp)/sizeof(centerp[0]); i++)
        tlc_tbl[centerp[i]] = TLC_CENTER_PAGE;

    for (i = 0; i < rows -2; i++) {
        for(j = 0; j < page_per_row; j+=2) {
            int idx = 8 + (i*page_per_row) + j;
            tlc_tbl[idx] = tlc_tbl[idx+1] = lpflag;
            lpflag = (lpflag == TLC_UPPER_PAGE) ? TLC_LOWER_PAGE : lpflag + 1;
        }
    }
}

// QLC-NAND Flash Mapping with Shadow-Page programming Sequence

void init_qlc_page_pairing(FemuCtrl *n)
{
	int i, j;
    int rows = (MAX_SUPPORTED_PAGES_PER_BLOCK + 7) / 8;
    int lpflag = QLC_LOWER_PAGE;
	int page_per_row=8;

    int lowp[] = {0, 1, 2, 3, 4, 5, 8, 9};
    int centerlp[] = {6, 7, 10, 11};
    int centerup[] = {12, 13};

    for (i = 0; i < sizeof(lowp)/sizeof(lowp[0]); i++)
        qlc_tbl[lowp[i]] = QLC_LOWER_PAGE;

    for (i = 0; i < sizeof(centerlp)/sizeof(centerlp[0]); i++)
        qlc_tbl[centerlp[i]] = QLC_LOWER_CENTER_PAGE;

    for (i = 0; i < sizeof(centerup)/sizeof(centerup[0]); i++)
        qlc_tbl[centerup[i]] = QLC_UPPER_CENTER_PAGE;

    for (i = 0; i < rows -3; i++) {
        for (j = 0; j < page_per_row; j+=2) {
            int idx = 8 + (i*page_per_row) + j;
            tlc_tbl[idx] = tlc_tbl[idx+1] = lpflag;
            lpflag = (lpflag == QLC_UPPER_PAGE) ? QLC_LOWER_PAGE : lpflag + 1;
        }
    }
}