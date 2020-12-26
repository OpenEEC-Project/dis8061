/***************************************************************
 *  806x disassembler - header file
 *
 *  source file --> dis_806x.h
 *
 *  Copyright   1997,1998   William Lawrance
 ***************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>



/***************************************************************
 *  maximums
 ***************************************************************/
#define MAXCMDS     1000            /* maximum # commands           */
#define MAXCMTS     2000            /* maximum # comments           */
#define MAXJCS      5000            /* maximum jump/calls           */



/***************************************************************
 * command definitions
 ***************************************************************/
#define DIS_ORG         1       /* origin           */
#define DIS_IGNORE      2       /* ignore data      */
#define DIS_HEX         3       /* hex              */
#define DIS_BYTES       4       /* bytes            */
#define DIS_WORDS       5       /* words            */
#define DIS_ASCII       6       /* ascii            */
#define DIS_CODE        7       /* code             */
#define DIS_LABEL       8       /* label            */
#define DIS_COMMENT     9       /* comment          */
#define DIS_COMMENTL    10      /* line comment     */




/***************************************************************
 *  predefined symbols
 ***************************************************************/
typedef struct 
{   char        *name;              /* name                     */
    unsigned    value;              /* value                    */
}   PREDEF_SYMS;



/***************************************************************
 *  command structure
 ***************************************************************/
typedef struct 
{   int         cmd;                /* command                  */
    int         cmd_seq;            /* command # from input     */
    unsigned    start;              /* start PC to apply        */
    unsigned    end;                /* end PC to apply          */
}   DISCMD;



/***************************************************************
 *  comment table entry
 *     typ = 1,  comment at end of a line
 *     typ = 2,  separate comment line
 ***************************************************************/
typedef struct
{   unsigned    pc;                 /* location to apply comment    */
    char        seq;                /* sequence #                   */
    char        typ;                /* comment type                 */
    char        *comnt;             /* the comment                  */
}   COMTBLE;



/***************************************************************
 *  jump/call reference entry
 ***************************************************************/
typedef struct
{   unsigned        dest;   /* destination of jump or call          */
                            /*  * upper byte is type (JC_xxx ...)   */
    unsigned        ref;    /* reference address                    */
}   JCENTRY;



/***************************************************************
 *  opcode table entry
 ***************************************************************/
typedef struct
{   char        numbyts;            /* # of bytes for instruction   */
    char        numops;             /* # of operands                */
    unsigned char   typ;            /* instruction type             */
    char        name[6];            /* the opcode name              */
}   OPCTBLE;

/*  instruction types - 'typ' field in OPCTBLE (bits 0-1)   */
#define DI  0x00    /* direct                       */
#define IM  0x01    /* immediate                    */
#define ID  0x02    /* indirect                     */
#define IX  0x03    /* index                        */
/*           not used - 'typ' field in OPCTBLE (bit  2  )   */
/*  instruction flags - 'typ' field in OPCTBLE (bits 3-7)   */
/*    'WR' is used only for 1-operand instructions          */
#define WR  0x08    /* instruction writes to dest   */
#define IW  0x10    /* immediate operand is WORD    */
#define PX  0x20    /* prefix byte is OK            */
#define UK  0x40    /* invalid opcode               */
#define SH  0x80    /* shift instruction            */



/***************************************************************
 *  jump/call instruction types
 ***************************************************************/
#define     JC_NONE     1   /* not a jump or call               */
#define     JC_CALL     2   /* call instruction                 */
#define     JC_RET      3   /* return instruction               */
#define     JC_JUMP     4   /* unconditional jump instruction   */
#define     JC_COND     5   /* conditional jump instruction     */



/***************************************************************
 *  references
 ***************************************************************/
extern  OPCTBLE     opctbl[];

extern int  find_name(unsigned addr, char **nam);
extern int  add_name(char *nam, unsigned addr);
extern int  init_name_by_pc(void);
extern int  next_name(unsigned pc, char **label);

extern int  build_label_array(DISCMD    cmds_array[],
                              int       cmd_count,
                              char      instr_array[],
                              char      labels_array[]);
extern int  load_predef_syms(void);
extern int  load_predef_cmds(void);
extern int  read_dirf       (FILE *fpdir);
extern int  proc_cmd        (char *cmdstr, int  seq_no);
extern int  ahextou         (char ahex[], unsigned *bin);
extern void set_bit  (char array_toset[], unsigned bitno);
extern int  check_bit(char array_tochk[], unsigned bitno);
extern int  add_jcent(unsigned dest, unsigned ref, int typ);
extern int  write_jcf(FILE *fp, JCENTRY jcents[], int cnt);


extern int  format_register (int  reg, int write, char *outp);
extern int  format_bitno    (int  bitno, char *outp);
extern int  format_imm_op   (char *bytep, int word_flg, char *outp);
extern int  format_idx_op   (char *bytep, int   long_flg, char  *outp);
extern int  format_address  (unsigned addr, char *outp);
extern int  format_2bytes   (unsigned val,  char *outp);
extern int  format_nbytes   (char *val, int cnt, char *outp);

extern long intr_rang        (int  typ,  char *buf, unsigned pc, long cnt, char labels[],
                                 COMTBLE comnts[], int no_cmts, int showhex, FILE *fpot);
extern int  intr_ign_line    (char *buf, unsigned pc, unsigned  cnt, char *outp);
extern int  intr_hex_line    (char *buf, unsigned pc, unsigned  cnt, char *outp);
extern int  intr_byte_line   (char *buf, unsigned pc, unsigned  cnt, char *outp);
extern int  intr_word_line   (char *buf, unsigned pc, unsigned  cnt, char *outp);
extern int  intr_ascii_line  (char *buf, unsigned pc, unsigned  cnt, char *outp);
extern int  intr_code_line   (char *buf, unsigned pc, unsigned  cnt, char *outp);
extern int  proc_instr_labels(char *buf, unsigned pc, char      labels[]);
