/***************************************************************
 *  806x disassembler - main routine
 *
 *  source file -->  main.c
 *
 *  Copyright   1997,1998   William Lawrance
 ***************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <dis_806x.h>


extern int cmd_compare  (const void *elem1, const void *elem2 );
extern int cmt_compare  (const void *elem1, const void *elem2 );
extern int jcent_compare(const void *elem1, const void *elem2 );


/**** program origin                */
unsigned    dis_org         = {0x2000};

/**** commands                      */
int         no_cmds         = {0};
DISCMD      cmds[MAXCMDS]   = {0};

/**** comments                      */
int         cmt_seq         = {0};  /* used to keep comment for same location
                                                in the original order       */
int         no_cmts         = {0};
COMTBLE     cmts[MAXCMTS]   = {0};

/**** jump/call's                   */
int         no_jcs          = {0};
JCENTRY     jcents[MAXJCS]  = {0};


/**** predefined symbols            */
PREDEF_SYMS   predef_syms[] =
{   "Zero",                 0x0000,
    "Low_Speed_Output",     0x0002,
    "BiDirect_Port",        0x0003,
    "AD_Lo",                0x0004,
    "Watchdog_Timer",       0x0004,
    "AD_Hi",                0x0005,
    "AD_Command",           0x0005,
    "Master_IO_Timer_Lo",   0x0006,
    "Master_IO_Timer_Hi",   0x0007,
    "Interrupt_Mask",       0x0008,
    "Interrupt_Pending",    0x0009,
    "IO_Status",            0x000a,
    "HSI_Sample",           0x000b,
    "HSI_Data_Mask",        0x000c,
    "HSI_Data_Hold",        0x000d,
    "HSI_Time_Hold",        0x000e,
    "Stack_Ptr",            0x0010,

    "Reset_Addr",           0x2000,
    "SMP_Base_Addr",        0x200c,
    "Calib_Exec_Time",      0x200e,
    "Int_Vec",              0x2010,
    "Num_levels",           0x2020,
    "Num_Calibs",           0x2021,
    "Calib_ptrs",           0x2022
};




/***************************************************************
 *  main routine  -  disassemble 806x object
 ***************************************************************/
int     main(int argc, char *argv[])

{   int         sts, i, showhex;
    unsigned    pc, inpbufl;
    long        inplen, cnt;
    char        *inpbuf, *basename, *suffix, *p;
    char        basepath[250];
    char        filin[250], filot[250], fildir[250], filjc[250];
    DISCMD      *cmd;
    PREDEF_SYMS *sp;
    FILE        *fpin, *fpot, *fpdir, *fpjc;
    char        outp[100];
    char        labels[8192];

    no_cmds = 0;
/* print opening banner                         */
    printf("\n**** 8061 Disassembler, Version 1.15 ****\n");
/* get input parameters                         */
    if(argc < 2  ||  3 < argc)
    {   fprintf(stderr, "Usage:  dis806x [-x]  <input_file>\n");
        fprintf(stderr, "    '-x' to suppress hex in output\n");
        exit(1);
    }
    if(argc == 2)           /* filename is only argument present        */
    {   p = argv[1];
        showhex = 1;
    }
    else                    /* another argument is present, check it    */
    {   p = argv[2];
        if(strcmp("-x", argv[1]))
        {   fprintf(stderr, "**** Unknown option: %s\n", argv[1]);
            exit(1);
        }
        showhex = 0;
    }
    if(strlen(p) > sizeof(filin) - 5)
    {   fprintf(stderr, "Input path too long: %s\n", p);
        exit(1);
    }
    strcpy(filin, p);

/* get base path, i.e. without extension        */
    strcpy(basepath, filin);
    basename = strrchr(basepath, '\\');     /* get ptr to last component    */
    if(basename == NULL)            /* no back slashes, i.e. just file name */
        basename = basepath;
    suffix = strrchr(basename, '.');    /* get ptr to suffix                */
    if(suffix)                          /* remove suffix from base          */
        *suffix = '\0';

/* fix input path, if no suffix specified       */
    if(!suffix)
        strcat(filin, ".bin");

/* build output listing path                    */
    strcpy(filot, basepath);
    if(showhex)
        strcat(filot, ".lst");      /* with hex output, use '.lst'      */
    else
        strcat(filot, ".asm");      /* without hex output, use '.asm'   */

/* build directive file path                    */
    strcpy(fildir, basepath);
    strcat(fildir, ".dir");

/* build jump/call output file path             */
    strcpy(filjc, basepath);
    strcat(filjc, ".jc");

/* open output/listing file                     */
    if((fpot = fopen(filot, "w")) == NULL)
    {   fprintf(stderr, "Can't open output: %s\n", filot);
        perror("listing file");
        exit(1);
    }

/* open jump/call output file                   */
    if((fpjc = fopen(filjc, "w")) == NULL)
    {   fprintf(stderr, "Can't open jump/call output: %s\n", filjc);
        perror("jump/call file");
        exit(1);
    }

/* load predefined symbols                      */
    if((sts = load_predef_syms( ) ) < 0)
    {   fprintf(stderr, "Can't load predefined symbols: %s\n", sts);
        exit(1);
    }

/* load predefined commands                     */
    if((sts = load_predef_cmds( ) ) < 0)
    {   fprintf(stderr, "**** Can't load predefined commands: %s\n", sts);
        exit(1);
    }

/* open directives file and process directives  */
    fpdir = fopen(fildir, "r");
    if(fpdir)
    {   if((sts = read_dirf(fpdir)) < 0)
        {   fprintf(stderr, "**** Error in directives: %d\n", sts);
            fclose(fpot);
            fclose(fpjc);
            fclose(fpdir);
            exit(1);
        }
        fclose(fpdir);
    }

/* allocate input buffer                        */
    inpbufl = 40000;
    if((inpbuf = malloc(inpbufl)) == NULL)
    {   fprintf(stderr, "**** Can't allocate input buffer\n");
        fclose(fpot);
        fclose(fpjc);
        exit(1);
    }

/* open input file and read the file            */
    if((fpin = fopen(filin, "rb")) == NULL)
    {   fprintf(stderr, "**** Can't open input: %s\n", filin);
        fclose(fpot);
        fclose(fpjc);
        exit(1);
    }
    inplen = (long) fread(inpbuf, 1, inpbufl, fpin);
    printf("Read %ld bytes from %s\n\n", inplen, filin);
    fclose(fpin);

/* sort directives and fill in CMD structures   */
    qsort(cmds, no_cmds, sizeof(DISCMD), cmd_compare);
    for(i = 0; i < no_cmds; ++i)
    {   if(i == no_cmds - 1)
            cmds[i].end = dis_org + inplen;
        else
            cmds[i].end = cmds[i + 1].start - 1;
    }

/* sort comments                                */
    qsort(cmts, no_cmts, sizeof(COMTBLE), cmt_compare);

/* build the label array                        */
    if((sts = build_label_array(cmds, no_cmds, inpbuf, labels)) < 0)
    {   fprintf(stderr, "**** Error in build labels: %d\n", sts);
  /* continue, ignoring this error      */
    }

/* init symbol table for use                    */
    if((sts = init_name_by_pc()) < 0)
    {   fprintf(stderr, "Error in init sym table: %s\n", sts);
        fclose(fpot);
        exit(1);
    }

/* output the ORG psuedo-op                     */
    memset(outp, ' ', 79);
    if((sts = format_2bytes(dis_org, &outp[0])) < 0)    /* put value in 1st 4 chars     */
        return(sts);
    memcpy(&outp[26], "org   ", 6);                     /* put op-code in output        */
    outp[32] = '$';
    if((sts = format_2bytes(dis_org, &outp[33])) < 0)
        return(sts);
    outp[37] = '\0';
    p = showhex ? &outp[0] : &outp[20];
    fprintf(fpot, "%s\n", p);

/* output the pre-defined symbols               */
    sp = predef_syms;
    for(i = 0; i < sizeof(predef_syms) / sizeof(PREDEF_SYMS); ++i)
    {   memset(outp, ' ', 79);
        if((sts = format_2bytes(sp->value, &outp[0])) < 0)  /* put value in 1st 4 chars     */
            return(sts);
        memcpy(&outp[20], sp->name, strlen(sp->name) );
        memcpy(&outp[54], "equ", 3);                        /* put op-code in output        */
        outp[60] = '$';
        if((sts = format_2bytes(sp->value, &outp[61])) < 0)
            return(sts);
        outp[65] = '\0';
        p = showhex ? &outp[0] : &outp[20];
        if(sp->value < dis_org)         /* display if not in disassembly                    */
            fprintf(fpot, "%s\n", p);
        ++sp;
    }

/* interpret the input and write output         */
    sts = 0;
    pc  = dis_org;
    for(i = 0; i < no_cmds; ++i)
    {   cmd = &cmds[i];
        if(cmd->start < pc)
        {   if(pc - cmd->start  <  8)   /* allow some overlap for       */
                ;                       /*    code disassembly past end */
            else
            {   sts = -1;
                fprintf(stderr, "**** Error: commands out of order\n");
                fprintf(stderr, "command #%d\n", i);
                break;
            }
        }
    /* interpret the data within range of this command              */
        if(cmd->end < cmd->start)
        {   sts = -2;
            fprintf(stderr, "**** Error: Command: end < start\n");
            fprintf(stderr, "    Command # %d\n", cmd->cmd_seq);
            break;
        }
        cnt = cmd->end - pc + 1;
        if(inplen < (long) ((pc - dis_org) + cnt))
            cnt = inplen - (pc - dis_org);
        if((cnt = intr_rang(cmd->cmd, &inpbuf[pc - dis_org], pc, cnt,
                                   labels, cmts, no_cmts, showhex, fpot)) < 0)
        {   sts = (int) cnt;
            break;
        }
        pc  += (unsigned) cnt;
    }
    if(sts < 0)             /* display any error encountered        */
        fprintf(stderr, "Error: %d\n", sts);

/* output the END psuedo-op                     */
    memset(outp, ' ', 79);
    if((sts = format_2bytes(pc, &outp[0])) < 0)         /* put value in 1st 4 chars     */
        return(sts);
    strcpy(&outp[26], "end");                           /* put op-code in output        */
    p = showhex ? &outp[0] : &outp[20];
    fprintf(fpot, "%s\n", p);

/* write the jump/call file                     */
    write_jcf(fpjc, jcents, no_jcs);

/* close output file        */
    fclose(fpot);

/* done                     */
    return(0);
}



/***************************************************************
 *  build label array
 *
 *  1. Process each command entry for 'code' type
 *  2. For each section of code, scan for call/jump instructions
 *  3. For each call/jump instruction, set the label array to cause
 *      a label to be output for the destination.
 *
 *  returns  zero  if successful
 *       OR  negative error code
 ***************************************************************/
int     build_label_array(DISCMD    cmds_array[],   /* command list     */
                          int       cmd_count,      /* # of commands    */
                          char      instr_array[],  /* instructions     */
                          char      labels_array[]) /* labels bit array */
    
{   int         cmdx, ilen;
    unsigned    pc;
    DISCMD      *cmdp;

    memset(labels_array, 0, 8192);
    for(cmdx = 0; cmdx < cmd_count; ++cmdx)
    {   if(cmds_array[cmdx].cmd == DIS_CODE)
        {   cmdp = &cmds_array[cmdx];
            for(pc = cmdp->start;  pc < cmdp->end;  pc += ilen)
            {   if((ilen =  proc_instr_labels(&instr_array[pc - dis_org],
                                                     pc, labels_array)) < 0)
                    return(ilen);
            }
        }
    }

/* sort the jump/call table entries     */
    qsort(jcents, no_jcs, sizeof(JCENTRY), jcent_compare);

    return(0);
}



/***************************************************************
 *  read directives file and process commands
 *
 *  returns 0 if successful
 *    OR negative error code
 ***************************************************************/
int     read_dirf(FILE *fpdir)

{   int     sts, i;
    char    buf[200];

    for(i = 1; i < 100000; ++i)
    {   if(!fgets(buf, sizeof(buf), fpdir))
            break;
        if((sts = proc_cmd(buf, i)) < 0)
        {   fprintf(stderr, "**** Error at line #%d in directive file\n", i);
            return(sts);
        }
    }
    return(0);
}



/***************************************************************
 *  process a command
 *
 *  returns 0 if successful
 *    OR negative error code
 ***************************************************************/
int     proc_cmd(char *cmdstr, int  seq_no)

{   int         sts, clen, len;
    unsigned    strt;
    char        *nam, *p, *txt;
    DISCMD      *cmd;
    COMTBLE     *cmt;

  /* remove trailing CR/LF and blanks from command  */
    clen = strlen(cmdstr);
    while(0 < clen  &&  cmdstr[clen - 1] <= ' ')
        cmdstr[--clen] = '\0';

  /* ignore empty command lines         */
    if(clen == 0)
        ;
  /* ignore comments-- ';' in col. 1    */
    else if(*cmdstr == ';')
        ;
  /* command too short                  */
    else if(clen < 16)
    {   fprintf(stderr, "**** Command too short: %s\n", cmdstr);
        return(-1);
    }

  /* process an 'ORG' command           */
    else if(!memcmp(cmdstr, "org         ", 12))
    {   if((sts = ahextou(&cmdstr[12], &dis_org)) < 0)
            return(sts);
    }
  /* process a 'LABEL' command          */
    else if(!memcmp(cmdstr, "label       ", 12))
    {   if((sts = ahextou(&cmdstr[12], &strt)) < 0)
            return(sts);
        if(clen < 18)
        {   fprintf(stderr, "Command too short: %s\n", cmdstr);
            exit(1);
        }
        nam = &cmdstr[17];
        if((p = strchr(nam, ' ')) != NULL)
            *p = '\0';
        if((sts = add_name(nam, strt)) < 0)
        {   fprintf(stderr, "Add Label error: %d\n", sts);
            exit(1);
        }
    }
  /* process comment commands           */
    else if(!memcmp(cmdstr, "comment     ", 12))
    {   if((sts = ahextou(&cmdstr[12], &strt)) < 0)
            return(sts);
        if(clen < 18)
        {   fprintf(stderr, "Command too short: %s\n", cmdstr);
            exit(1);
        }
        if(MAXCMTS <= no_cmts)
        {   fprintf(stderr, "****ERROR, too many comments\n");
            exit(1);
        }
        cmt = &cmts[no_cmts++];
        txt = &cmdstr[17];
        len = strlen(txt) + 1 + 1;  /* 1 for NULL, 1 for possible ';'   */
        if(!(cmt->comnt = malloc(len)))
        {   fprintf(stderr, "****ERROR, can't allocate memory for comments\n");
            exit(1);
        }
        cmt->pc  = strt;
        cmt->seq = ++cmt_seq;
        cmt->typ = 1;
        if(*txt != ';')                 /* insert ';' first if none present     */
        {   strcpy(cmt->comnt, ";");
            strcat(cmt->comnt, txt);
        }
        else
            strcpy(cmt->comnt, txt);
    }
    else if(!memcmp(cmdstr, "commentl    ", 12))
    {   if((sts = ahextou(&cmdstr[12], &strt)) < 0)
            return(sts);
        if(clen < 18)
        {   fprintf(stderr, "Command too short: %s\n", cmdstr);
            exit(1);
        }
        if(MAXCMTS <= no_cmts)
        {   fprintf(stderr, "****ERROR, too many comments\n");
            exit(1);
        }
        cmt = &cmts[no_cmts++];
        txt = &cmdstr[17];
        len = strlen(txt) + 1 + 1;      /* 1 for NULL, 1 for possible ';'       */
        if(!(cmt->comnt = malloc(len)))
        {   fprintf(stderr, "****ERROR, can't allocate memory for comments\n");
            exit(1);
        }
        cmt->pc  = strt;
        cmt->seq = ++cmt_seq;
        cmt->typ = 2;
        if(*txt != ';')         /* insert ';' first if none present     */
        {   strcpy(cmt->comnt, ";");
            strcat(cmt->comnt, txt);
        }
        else
            strcpy(cmt->comnt, txt);
    }

  /* all other commands need a command entry    */
    else
    {   
      /**** get a new command entry             */
        if(MAXCMDS <= no_cmds)
        {   fprintf(stderr, "****ERROR, Too many commands\n");
            exit(1);
        }
        cmd = &cmds[no_cmds++];
        cmd->cmd_seq = seq_no;
              
      /**** process other commands              */  
        if(!memcmp(cmdstr, "ignore      ", 12))
        {   cmd->cmd = DIS_IGNORE;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "hex         ", 12))
        {   cmd->cmd = DIS_HEX;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "bytes       ", 12))
        {   cmd->cmd = DIS_BYTES;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "byte        ", 12))
        {   cmd->cmd = DIS_BYTES;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "words       ", 12))
        {   cmd->cmd = DIS_WORDS;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "word        ", 12))
        {   cmd->cmd = DIS_WORDS;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "ascii       ", 12))
        {   cmd->cmd = DIS_ASCII;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else if(!memcmp(cmdstr, "code        ", 12))
        {   cmd->cmd = DIS_CODE;
            if((sts = ahextou(&cmdstr[12], &cmd->start)) < 0)
                return(sts);
        }
        else
        {   fprintf(stderr, "Unknown command: %s", cmdstr);
            exit(1);
        }
    }

  /* done - return              */
    return(0);
}



/***************************************************************
 *  load pre-defined commands
 *
 *  returns 0 if successful
 *    OR negative error code
 ***************************************************************/
int     load_predef_cmds( )
{   int         sts, i;
    static char *predef_cmds[]  =
    {   "org         2000"
    };

    for(i = 0; i < sizeof(predef_cmds) / sizeof(predef_cmds[0]); ++i)
        if((sts = proc_cmd(predef_cmds[i], 30000 + i)) < 0)
            return(sts);
    return(0);
}



/***************************************************************
 *  load pre-defined symbols
 *
 *  returns 0 if successful
 *    OR negative error code
 ***************************************************************/
int     load_predef_syms( )
{   int         i, sts;
    PREDEF_SYMS *sp;

    sp = predef_syms;
    for(i = 0; i < sizeof(predef_syms) / sizeof(PREDEF_SYMS); ++i)
    {   if((sts = add_name(sp->name, sp->value)) < 0)
            return(sts);
        ++sp;
    }
    return(0);
}



/***************************************************************
 *  compare routine for sort of 'cmds'  array
 *
 *  returns >=0 if first element is 'greater than or equal' second element
 *    OR    < 0  if first element is 'less than' second element
 ***************************************************************/
int     cmd_compare(const void *elem1, const void *elem2 )
{   return(  ((DISCMD *) elem1)->start
                -  ((DISCMD *) elem2)->start  );
}



/***************************************************************
 *  compare routine for sort of 'cmts'  array
 *
 *  returns >0 if first element is 'greater than' second element
 *    OR     0 if first element is 'equal' second element
 *    OR    <0 if first element is 'less than' second element
 ***************************************************************/
int     cmt_compare(const void *elem1, const void *elem2 )

{   int     tdiff;
    COMTBLE *cmt1, *cmt2;

    cmt1 = (COMTBLE *) elem1;
    cmt2 = (COMTBLE *) elem2;

  /* PC is 1st key                      */
    if(cmt1->pc > cmt2->pc)
        return(1);
    if(cmt1->pc < cmt2->pc)
        return(-1);

  /* comment type is 2nd key            */
  /* sort reverse, want type 2's first  */
    if((tdiff = cmt1->typ - cmt2->typ) != 0)
        return(-tdiff);

  /* seq # is 3rd key                   */
    return(cmt1->seq - cmt2->seq);
}



/***************************************************************
 *  compare routine for sort of 'jcents'  array
 *
 *  returns >0 if first element is 'greater than' second element
 *    OR     0 if first element is 'equal' second element
 *    OR    <0 if first element is 'less than' second element
 ***************************************************************/
int     jcent_compare(const void *elem1, const void *elem2 )

{   unsigned    dest1, dest2, ref1, ref2;
    JCENTRY     *ent1, *ent2;

    ent1 = (JCENTRY *) elem1;
    ent2 = (JCENTRY *) elem2;

  /* destination has type in 1st byte   */
    dest1 = ent1->dest & 0xffffff;
    dest2 = ent2->dest & 0xffffff;

  /* dest is 1st key                    */
    if(dest1 > dest2)
        return(1);
    if(dest1 < dest2)
        return(-1);

  /* referencing location is 2nd key    */
  /* type is in 1st byte as part of key */
    ref1 = ent1->ref;
    ref2 = ent2->ref;
    if(ref1  > ref2)
        return(1);
    if(ref1 < ref2)
        return(-1);

  /* shouldn't be equal                 */
    return(0);
}



/***************************************************************
 *  convert ASCII hex to unsigned int
 *     converts 4 ASCII characters
 *
 *  returns 0 if successful
 *    OR negative error code
 ***************************************************************/
int     ahextou(char        ahex[],     /* ascii hex to convert     */
                unsigned    *bin)       /* output                   */

{   int         i, dig;
    unsigned    rslt;
    char        *p;
    static char *hexc   = {"0123456789abcdef"};

    rslt = 0;
    for(i = 0; i < 4; ++i)
    {   if((p = strchr(hexc, tolower(ahex[i]))) == NULL)
        {   fprintf(stderr, "Illegal hex char: %c\n", ahex[i]);
            return(-1);
        }
        dig = p - hexc;
        rslt <<= 4;
        rslt  |= dig;
    }
    *bin = rslt;
    return(0);
}



/***************************************************************
 *  set bit in an array
 *
 *  array is assumed to be 64k bits (i.e. 8192 bytes) long
 ***************************************************************/
void    set_bit(char array_toset[], unsigned   pc)
{   int         ix;
    static char mask[] = { (char)0x80, (char)0x40, (char)0x20, (char)0x10,
                           (char)0x08, (char)0x04, (char)0x02, (char)0x01};

    pc &= 0xffff;
    ix  = (pc >> 3) & 0x1fff;
    array_toset[ix]  |=  mask[pc & 0x07];
}



/***************************************************************
 *  check bit in an array
 *
 *  array is assumed to be 64k bits (i.e. 8192 bytes) long
 *
 *  returns  non-zero  if bit is set
 *       OR  zero      if bit isn't set
 ***************************************************************/
int     check_bit(char array_tocheck[], unsigned   pc)
{   int         ix;
    static char mask[] = { (char)0x80, (char)0x40, (char)0x20, (char)0x10,
                           (char)0x08, (char)0x04, (char)0x02, (char)0x01};

    pc &= 0xffff;
    ix  = (pc >> 3) & 0x1fff;
    return(array_tocheck[ix]  &  mask[pc & 0x07]);
}



/***************************************************************
 *  add entry to jump call buffer
 *
 ***************************************************************/
int     add_jcent(unsigned  dest,
                  unsigned  ref,
                  int       jctyp)
                  
{   JCENTRY     *jcnew;
    static int  error_rep   = 0;

/* check that buffer has room           */
    if(MAXJCS <= no_jcs)
    {   if(!error_rep)      /* report error first time      */
            fprintf(stderr, "**** Jump/Call buffer full; continuing\n");
        error_rep = 1;
        return(-1);
    }
/* get pointer to new entry             */
    jcnew = &jcents[no_jcs++];
/* fill it in                           */
    jcnew->dest  = dest;
    jcnew->dest |= jctyp << 24;     /* put type in upper byte   */
    jcnew->ref   = ref;
/* done - return                        */
    return(0);
}



/***************************************************************
 *  write jump/call output file
 ***************************************************************/
int     write_jcf(FILE      *fpot,
                  JCENTRY   jcents[],
                  int       cnt)
{   int         ix, lcnt, jtyp;
    unsigned    prev_dest, dest, ref;
    JCENTRY     *jcent;
    char        *p, val[4], output[200];

    fprintf(fpot, "\n");
    fprintf(fpot, "******** Jump/Call Report *********\n");
    fprintf(fpot, "  Legend:    -CA  reference is a 'call'\n");
    fprintf(fpot, "             -JU  reference is an unconditional jump\n");
    fprintf(fpot, "             -JC  reference is a conditional jump\n");
    fprintf(fpot, "\n");

    prev_dest = ~0;
    lcnt      = 0;
    for(ix = 0; ix < cnt; ++ix)     /* scan through entries         */
    {   jcent = &jcents[ix];
        ref   =  jcent->ref;
        dest  = (jcent->dest)       & 0xffffff;
        jtyp  = (jcent->dest >> 24) & 0xff;
        if(dest == prev_dest)
  /* same destination                       */
        {   if(6 <= lcnt)
            {   output[12 * lcnt] = '\0';
                fprintf(fpot, "%s\n", output);
                lcnt = 0;
            }
            val[0] = ref >> 8;
            val[1] = ref;
            p = &output[12 * lcnt + 4];
            format_nbytes(val, 2, p);
            switch(jtyp)
            {   case JC_CALL:
                    memcpy(p + 4, "-CA", 3);
                    break;
                case JC_JUMP:
                    memcpy(p + 4, "-JU", 3);
                    break;
                case JC_COND:
                    memcpy(p + 4, "-JC", 3);
                    break;
            }
            ++lcnt;
        }
        else
  /* new destination                        */
        {   if(prev_dest != ~0);            /* print previous line  */
            {   output[12 * lcnt] = '\0';
                fprintf(fpot, "%s\n", output);
            }
        /* print header line                */
            strcpy(output, "**** References to: ");
            p  = output + strlen(output);
            p += format_address(dest, p);
            *p = '\0';
            fprintf(fpot, "\n%s\n", output);
        /* initialize output line and vars  */
            memset(output, ' ', sizeof(output) );
            output[100] = '\0';
            lcnt = 0;
        /* put reference in new output line */
            val[0] = ref >> 8;
            val[1] = ref;
            p = &output[12 * lcnt + 4];
            format_nbytes(val, 2, p);
            switch(jtyp)
            {   case JC_CALL:
                    memcpy(p + 4, "-CA", 3);
                    break;
                case JC_JUMP:
                    memcpy(p + 4, "-JU", 3);
                    break;
                case JC_COND:
                    memcpy(p + 4, "-JC", 3);
                    break;
            }
            ++lcnt;
            prev_dest = dest;
        }
    }

/* print last line          */
    output[12 * lcnt] = '\0';
    fprintf(fpot, "%s\n", output);

/* done - return            */
    return(0);
}
