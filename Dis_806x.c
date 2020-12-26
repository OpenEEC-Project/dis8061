/***************************************************************
 *  806x disassembler - disassembly routines
 *
 *  source file -->  "dis_806x.c"
 *
 *  Copyright   1997,1998   William Lawrance
 ***************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <dis_806x.h>




/***************************************************************
 *  the opcode table
 ***************************************************************/
OPCTBLE     opctbl[]    =   {
2,1,DI      ,"skip  ",  2,1,DI      +WR,"clr   ",  2,1,DI   +WR,"not   ",  2,1,DI      +WR,"neg   ",/* 000 */
1,0,   UK   ,"      ",  2,1,DI      +WR,"dec   ",  2,1,DI   +WR,"ext   ",  2,1,DI      +WR,"inc   ",/* 004 */
3,2,   SH   ,"shr   ",  3,2,      SH   ,"shl   ",  3,2,   SH   ,"shra  ",  1,0,      UK,   "      ",/* 008 */
3,2,   SH   ,"shrl  ",  3,2,      SH   ,"shll  ",  3,2,   SH   ,"shral ",  3,2,DI         ,"norml ",/* 00c */
1,0,   UK   ,"      ",  2,1,DI      +WR,"clrb  ",  2,1,DI   +WR,"notb  ",  2,1,DI      +WR,"negb  ",/* 010 */
1,0,   UK   ,"      ",  2,1,DI      +WR,"decb  ",  2,1,DI   +WR,"extb  ",  2,1,DI      +WR,"incb  ",/* 014 */
3,2,   SH   ,"shrb  ",  3,2,      SH   ,"shlb  ",  3,2,   SH   ,"shrab ",  1,0,      UK   ,"      ",/* 018 */
1,0,   UK   ,"      ",  1,0,      UK   ,"      ",  1,0,   UK   ,"      ",  1,0,      UK   ,"      ",/* 01c */

2,1,0    ,"sjmp  ",  2,1,0       ,"sjmp  ",  2,1,0    ,"sjmp  ",  2,1,0       ,"sjmp  ",/* 020 */
2,1,0    ,"sjmp  ",  2,1,0       ,"sjmp  ",  2,1,0    ,"sjmp  ",  2,1,0       ,"sjmp  ",/* 024 */
2,1,0    ,"scall ",  2,1,0       ,"scall ",  2,1,0    ,"scall ",  2,1,0       ,"scall ",/* 028 */
2,1,0    ,"scall ",  2,1,0       ,"scall ",  2,1,0    ,"scall ",  2,1,0       ,"scall ",/* 02c */
3,1,0    ,"jbc   ",  3,1,0       ,"jbc   ",  3,1,0    ,"jbc   ",  3,1,0       ,"jbc   ",/* 030 */
3,1,0    ,"jbc   ",  3,1,0       ,"jbc   ",  3,1,0    ,"jbc   ",  3,1,0       ,"jbc   ",/* 034 */
3,1,0    ,"jbs   ",  3,1,0       ,"jbs   ",  3,1,0    ,"jbs   ",  3,1,0       ,"jbs   ",/* 038 */
3,1,0    ,"jbs   ",  3,1,0       ,"jbs   ",  3,1,0    ,"jbs   ",  3,1,0       ,"jbs   ",/* 03c */
4,3,DI   ,"and   ",  5,3,IM+IW   ,"and   ",  4,3,ID   ,"and   ",  5,3,IX      ,"and   ",/* 040 */
4,3,DI   ,"add   ",  5,3,IM+IW   ,"add   ",  4,3,ID   ,"add   ",  5,3,IX      ,"add   ",/* 044 */
4,3,DI   ,"sub   ",  5,3,IM+IW   ,"sub   ",  4,3,ID   ,"sub   ",  5,3,IX      ,"sub   ",/* 048 */
4,3,DI+PX,"mulu  ",  5,3,IM+IW+PX,"mulu  ",  4,3,ID+PX,"mulu  ",  5,3,IX   +PX,"mulu  ",/* 04c */
4,3,DI   ,"andb  ",  4,3,IM      ,"andb  ",  4,3,ID   ,"andb  ",  5,3,IX      ,"andb  ",/* 050 */
4,3,DI   ,"addb  ",  4,3,IM      ,"addb  ",  4,3,ID   ,"addb  ",  5,3,IX      ,"addb  ",/* 054 */
4,3,DI   ,"subb  ",  4,3,IM      ,"subb  ",  4,3,ID   ,"subb  ",  5,3,IX      ,"subb  ",/* 058 */
4,3,DI+PX,"mulub ",  4,3,IM   +PX,"mulub ",  4,3,ID+PX,"mulub ",  5,3,IX   +PX,"mulub ",/* 05c */
3,2,DI   ,"and   ",  4,2,IM+IW   ,"and   ",  3,2,ID   ,"and   ",  4,2,IX      ,"and   ",/* 060 */
3,2,DI   ,"add   ",  4,2,IM+IW   ,"add   ",  3,2,ID   ,"add   ",  4,2,IX      ,"add   ",/* 064 */
3,2,DI   ,"sub   ",  4,2,IM+IW   ,"sub   ",  3,2,ID   ,"sub   ",  4,2,IX      ,"sub   ",/* 068 */
3,2,DI+PX,"mulu  ",  4,2,IM+IW+PX,"mulu  ",  3,2,ID+PX,"mulu  ",  4,2,IX   +PX,"mulu  ",/* 06c */
3,2,DI   ,"andb  ",  3,2,IM      ,"andb  ",  3,2,ID   ,"andb  ",  4,2,IX      ,"andb  ",/* 070 */
3,2,DI   ,"addb  ",  3,2,IM      ,"addb  ",  3,2,ID   ,"addb  ",  4,2,IX      ,"addb  ",/* 074 */
3,2,DI   ,"subb  ",  3,2,IM      ,"subb  ",  3,2,ID   ,"subb  ",  4,2,IX      ,"subb  ",/* 078 */
3,2,DI+PX,"mulub ",  3,2,IM   +PX,"mulub ",  3,2,ID+PX,"mulub ",  4,2,IX   +PX,"mulub ",/* 07c */
3,2,DI   ,"or    ",  4,2,IM+IW   ,"or    ",  3,2,ID   ,"or    ",  4,2,IX      ,"or    ",/* 080 */
3,2,DI   ,"xor   ",  4,2,IM+IW   ,"xor   ",  3,2,ID   ,"xor   ",  4,2,IX      ,"xor   ",/* 084 */
3,2,DI   ,"cmp   ",  4,2,IM+IW   ,"cmp   ",  3,2,ID   ,"cmp   ",  4,2,IX      ,"cmp   ",/* 088 */
3,2,DI+PX,"divu  ",  4,2,IM+IW+PX,"divu  ",  3,2,ID+PX,"divu  ",  3,2,IX   +PX,"divu  ",/* 08c */
3,2,DI   ,"orb   ",  3,2,IM      ,"orb   ",  3,2,ID   ,"orb   ",  4,2,IX      ,"orb   ",/* 090 */
3,2,DI   ,"xorb  ",  3,2,IM      ,"xorb  ",  3,2,ID   ,"xorb  ",  4,2,IX      ,"xorb  ",/* 094 */
3,2,DI   ,"cmpb  ",  3,2,IM      ,"cmpb  ",  3,2,ID   ,"cmpb  ",  4,2,IX      ,"cmpb  ",/* 098 */
3,2,DI+PX,"divub ",  3,2,IM   +PX,"divub ",  3,2,ID+PX,"divub ",  4,2,IX   +PX,"divub ",/* 09c */
3,2,DI   ,"ld    ",  4,2,IM+IW   ,"ld    ",  3,2,ID   ,"ld    ",  4,2,IX      ,"ld    ",/* 0a0 */
3,2,DI   ,"addc  ",  4,2,IM+IW   ,"addc  ",  3,2,ID   ,"addc  ",  4,2,IX      ,"addc  ",/* 0a4 */
3,2,DI   ,"subc  ",  4,2,IM+IW   ,"subc  ",  3,2,ID   ,"subc  ",  4,2,IX      ,"subc  ",/* 0a8 */
3,2,DI   ,"ldbze ",  3,2,IM      ,"ldbze ",  3,2,ID   ,"ldbze ",  4,2,IX      ,"ldbze ",/* 0ac */
3,2,DI   ,"ldb   ",  3,2,IM      ,"ldb   ",  3,2,ID   ,"ldb   ",  4,2,IX      ,"ldb   ",/* 0b0 */
3,2,DI   ,"addcb ",  3,2,IM      ,"addcb ",  3,2,ID   ,"addcb ",  4,2,IX      ,"addcb ",/* 0b4 */
3,2,DI   ,"subcb ",  3,2,IM      ,"subcb ",  3,2,ID   ,"subcb ",  4,2,IX      ,"subcb ",/* 0b8 */
3,2,DI   ,"ldbse ",  3,2,IM      ,"ldbse ",  3,2,ID   ,"ldbse ",  4,2,IX      ,"ldbse ",/* 0bc */
3,2,DI   ,"st    ",  1,0,      UK,"      ",  3,2,ID   ,"st    ",  4,2,IX      ,"st    ",/* 0c0 */
3,2,DI   ,"stb   ",  1,0,      UK,"      ",  3,2,ID   ,"stb   ",  4,2,IX      ,"stb   ",/* 0c4 */
2,1,DI   ,"push  ",  3,1,IM+IW   ,"push  ",  2,1,ID   ,"push  ",  3,1,IX      ,"push  ",/* 0c8 */
2,1,DI   ,"pop   ",  1,0,      UK,"      ",  2,1,ID   ,"pop   ",  3,1,IX      ,"pop   ",/* 0cc */
2,1, 0   ,"jnst  ",  2,1, 0      ,"jnh   ",  2,1, 0   ,"jgt   ",  2,1, 0      ,"jnc   ",/* 0d0 */
2,1, 0   ,"jnvt  ",  2,1, 0      ,"jnv   ",  2,1, 0   ,"jge   ",  2,1, 0      ,"jne   ",/* 0d4 */
2,1, 0   ,"jst   ",  2,1, 0      ,"jh    ",  2,1, 0   ,"jle   ",  2,1, 0      ,"jc    ",/* 0d8 */
2,1, 0   ,"jvt   ",  2,1, 0      ,"jv    ",  2,1, 0   ,"jlt   ",  2,1, 0      ,"je    ",/* 0dc */
3,1, 0   ,"djnz  ",  1,0,      UK,"      ",  1,0,   UK,"      ",  2,1,DI      ,"br[]  ",/* 0e0 */
1,0,   UK,"      ",  1,0,      UK,"      ",  1,0,   UK,"      ",  3,1, 0      ,"ljmp  ",/* 0e4 */
1,0,   UK,"      ",  1,0,      UK,"      ",  1,0,   UK,"      ",  1,0,      UK,"      ",/* 0e8 */
1,0,   UK,"      ",  1,0,      UK,"      ",  1,0,   UK,"      ",  3,1, 0      ,"lcall ",/* 0ec */
1,0, 0   ,"ret   ",  1,0, 0      ,"reti  ",  1,0, 0   ,"pushf ",  1,0, 0      ,"popf  ",/* 0f0 */
1,0,   UK,"      ",  1,0,      UK,"      ",  1,0,   UK,"      ",  1,0,      UK,"      ",/* 0f4 */
1,0, 0   ,"clrc  ",  1,0, 0      ,"setc  ",  1,0, 0   ,"di    ",  1,0, 0      ,"ei    ",/* 0f8 */
1,0, 0   ,"clrvt ",  1,0,      UK,"      ",  1,0,   UK,"      ",  1,0, 0      ,"nop   ",/* 0fc */

    0,0,0,"\0\0\0\0"};    /* end  */




/***************************************************************
 *  interpret a range of PC's
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
long    intr_rang(  int         typ,        /* type of interpretation   */
                    char        *ibuf,      /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    long        inpcnt,     /* # of bytes to interpret  */
                    char        labels[],   /* labels array             */
                    COMTBLE     cmts[],     /* comments                 */
                    int         no_cmts,    /* no of comments in array  */
                    int         show_hex,   /* show hex data            */
                    FILE        *fpot)      /* output FILE              */

{   int         sts, i, j, ibx, cnt, dig, cmtx;
    long        inpx;
    char        *label, *p, *leading, outbuf[200];
    COMTBLE     *cmt;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};
    static char blanks[]
          = "                                                            ";
    leading = show_hex ? "                    " : "";
/* find position in comment array                   */
    for(cmtx = 0; cmtx < no_cmts; ++cmtx)
        if(pc <= cmts[cmtx].pc)
            break;

/* process range requested                          */
    for(inpx = 0; inpx < inpcnt; )
    {
  /* display any line comments for the current PC   */
        for( ; cmtx < no_cmts; ++cmtx)
        {   cmt = &cmts[cmtx];
            if(pc < cmt->pc)        /* no more for this PC              */
                break;
            if(pc > cmt->pc)        /* was skipped, doesn't align       */
                fprintf(fpot, "%s**** Comment not aligned, PC=%x, %s\n",
                                                leading, cmt->pc, cmt->comnt);
            else                    /* this comment for this PC         */
            {   if(cmt->typ == 1)           /* all line comments done   */
                    break;
                else if(cmt->typ == 2)      /* this is a line comment   */
                    fprintf(fpot, "%s%s\n", leading, cmt->comnt);
            }
        }
    
  /* display any labels present for the current PC  */
        while(0 <= (sts = next_name(pc, &label)))
        {   strcpy(outbuf, leading);
            if(show_hex)
            {  if((sts = format_2bytes(pc, &outbuf[0])) < 0)
                    return(sts);
            }
            strcat(outbuf, label);
            fprintf(fpot, "%s\n", outbuf);
        }

  /* determine how far to next label or comment     */

        
  /* display a line of output                       */
        switch(typ)
        {   case    DIS_IGNORE:
                cnt = intr_ign_line(  &ibuf[inpx], pc, (unsigned) (inpcnt - inpx), outbuf);
                break;
            case    DIS_HEX:
                cnt = intr_hex_line(  &ibuf[inpx], pc, (unsigned) (inpcnt - inpx), outbuf);
                break;
            case    DIS_BYTES:
                cnt = intr_byte_line( &ibuf[inpx], pc, (unsigned) (inpcnt - inpx), outbuf);
                break;
            case    DIS_WORDS:
                cnt = intr_word_line( &ibuf[inpx], pc, (unsigned) (inpcnt - inpx), outbuf);
                break;
            case    DIS_ASCII:
                cnt = intr_ascii_line(&ibuf[inpx], pc, (unsigned) (inpcnt - inpx), outbuf);
                break;
            case    DIS_CODE:
                cnt = intr_code_line( &ibuf[inpx], pc, (unsigned) (inpcnt - inpx), outbuf);
                break;
            default:
                fprintf(stderr, "****Error: Bad Command Table entry\n");
                exit(1);
                break;
        }
        if(cnt < 0)
        {   fprintf(stderr, "****Error in interpret: %d\n", cnt);
            exit(1);
        }
    /* add a generated label if needed      */
        if(check_bit(labels, pc))
        {   p = &outbuf[20];
            *p++ = 'L';
            format_2bytes(pc, p);
        }

    /* remove trailing blanks from line     */
        for(i = strlen(outbuf); 0 < i; --i)
            if(outbuf[i - 1] != ' ')
                break;
        outbuf[i] = '\0';

    /* add end of line comments             */
        if(pc == cmt->pc)           /* comment present for this PC          */
        {   if(cmt->typ == 1)           /* its an end of line comment       */
            {   i = strlen(outbuf);
                if(i < 60)                  /* fill out to 60               */
                    strcat(outbuf, blanks + i);
                else                        /* just add 4, if > 60 now      */
                    strcat(outbuf, "    ");
                strcat(outbuf, cmt->comnt);
                ++cmtx;             /* over this one                        */
            }
            i= cmtx;                /* skip any other comments for this PC  */
            while(cmtx < no_cmts)
            {   cmt = &cmts[cmtx];
                if(pc == cmt->pc)
                    ++cmtx;
                else
                    break;
            }
            if(i != cmtx)
                fprintf(stderr, "**** ERROR, %d comments skipped at PC=%4.4x\n", cmtx - i, pc);
        }
    /* print 1 line from 'intr' routine     */
        if(!show_hex)                       /* dont' show 1st 20 columns    */
        {   if(strlen(outbuf) < 20)             /* rec is short, use null   */
                p = "";
            else                                /* normal, skip 1st 20      */
                p = &outbuf[20];
        }
        else                                /* show hex output              */
            p = outbuf;
        fprintf(fpot, "%s\n", p);           /* print the line               */

    /* only 4 bytes printed, print rest; not for HEX or IGNORE  */
        if(show_hex  &&  typ != DIS_HEX  &&  typ != DIS_IGNORE)
        {   for(j = 4; j < cnt; j += 8)
            {   memset(outbuf, ' ', 79);
                outbuf[79] = '\0';
                p = &outbuf[6];    
                for(i = 0; i + j < cnt  &&  i < 8; ++i)
                {   ibx = (int) inpx + j + i;
                    dig  = (ibuf[ibx] >> 4) & 0xf;  /* get first digit          */
                    *p++ = hex[dig];                /* char. to display         */
                    dig  = (ibuf[ibx]     ) & 0xf;  /* get 2nd digit            */
                    *p++ = hex[dig];                /* char to display          */
                }
                *p++ = '\0';
                fprintf(fpot, "%s\n", outbuf);   /* print the line               */
            }
        }

    /* incr counts and loop                 */        
        pc   += cnt;
        inpx += cnt;
    }
    return(inpx);
}

               



/***************************************************************
 *  interpret a line of output as 'ignore'
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
int intr_ign_line(  char        *buf,       /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    unsigned    inpcnt,     /* max # bytes              */
                    char        *outp)      /* output buffer            */
               
{   int         sts;

    memset(outp, ' ', 79);
    
  /* put PC in 1st 4 chars      */
    if((sts = format_2bytes(pc, &outp[0])) < 0)
        return(sts);

  /* put DS op-code in output   */
    memcpy(&outp[26], "ds", 2);
    outp[32] = '$';
    if((sts = format_2bytes(inpcnt, &outp[33])) < 0)
        return(sts);
    strcpy(&outp[40], "; bytes ignored by 'ignore' command");

  /* return                     */
    return(inpcnt);
}               

               



/***************************************************************
 *  interpret a line of output as 'hex'
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
int intr_hex_line(  char        *buf,       /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    unsigned    inpcnt,     /* max # bytes              */
                    char        *outp)      /* output buffer            */
               
{   int         i, sts, ch, dig, cnt;
    char        *p;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

    cnt =  inpcnt < 8  ?  inpcnt  :  8;
    memset(outp, ' ', 79);
    outp[79] = '\0';
    
  /* put PC in 1st 4 chars      */
    if((sts = format_2bytes(pc, &outp[0])) < 0)
        return(sts);

  /* put hex object in output   */
    if((sts = format_nbytes(&buf[0], cnt < 4 ? cnt : 4, &outp[6])) < 0)
        return(sts);

  /* put op-code in output      */
    memcpy(&outp[26], "byte  ", 6);

  /* display hex characters     */
    p = &outp[32];
    for(i = 0; i < cnt; ++i)
    {   *p++ = '$';
        dig  = (buf[i] >> 4) & 0xf;     /* get first digit          */
        p[0] = hex[dig];                /* char. to display         */
        dig  = (buf[i]     ) & 0xf;     /* get 2nd digit            */
        p[1] = hex[dig];                /* char to display          */
        if(i < cnt - 1)                 /* comma, unless its last   */
            p[2] = ',';
        p += 3;
    }

  /* display print chars        */
    outp[66] = ';';                     /* make it a comment        */
    p = &outp[67];
    for(i = 0; i < cnt; ++i)
    {   ch = buf[i];
        if(ch < ' ' ||  '\x7f' <= ch)
            p[i] = '.';
        else
            p[i] = (char) ch;
    }

  /* return                     */
    return(cnt);
}               
               



/***************************************************************
 *  interpret a line of output as 'byte'
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
int intr_byte_line( char        *buf,       /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    unsigned    inpcnt,     /* max # bytes              */
                    char        *outp)      /* output buffer            */
               
{   int         i, sts, dig, cnt;
    char        *p;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

    cnt =  inpcnt < 1  ?  inpcnt  :  1;
    memset(outp, ' ', 79);
    outp[79] = '\0';
    
  /* put PC in 1st 4 chars      */
    if((sts = format_2bytes(pc, &outp[0])) < 0)
        return(sts);

  /* put hex object in output   */
    if((sts = format_nbytes(&buf[0], cnt < 4 ? cnt : 4, &outp[6])) < 0)
        return(sts);

  /* put op-code in             */
    memcpy(&outp[26], "byte  ", 6);

  /* display hex characters     */
    p = &outp[32];
    for(i = 0; i < cnt; ++i)
    {   *p++ = '$';
        dig  = (buf[i] >> 4) & 0xf;     /* get first digit          */
        p[0] = hex[dig];                /* char. to display         */
        dig  = (buf[i]     ) & 0xf;     /* get 2nd digit            */
        p[1] = hex[dig];                /* char to display          */
        if(i < cnt - 1)                 /* comma, unless its last   */
            p[2] = ',';
        p += 3;
    }

  /* return                     */
    return(cnt);
}               
               



/***************************************************************
 *  interpret a line of output as 'word'
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
int intr_word_line( char        *buf,       /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    unsigned    inpcnt,     /* max # bytes              */
                    char        *outp)      /* output buffer            */
               
{   int         i, sts, cnt, len;
    unsigned    val;
    char        *p, *nam, revb[2];
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

  /* init output buffer         */
    memset(outp, ' ', 79);
    outp[79] = '\0';

  /* get # bytes to interpret   */
    cnt =  inpcnt < 2  ?  inpcnt  :  2;     /* max of 2 bytes   */
    cnt =  (cnt + 1) & ~1;                  /* must be even     */
    
  /* put PC in 1st 4 chars      */
    if((sts = format_2bytes(pc, &outp[0])) < 0)
        return(sts);

  /* put hex object in output   */
    if((sts = format_nbytes(&buf[0], cnt < 4 ? cnt : 4, &outp[6])) < 0)
        return(sts);

  /* put op-code in output      */
    memcpy(&outp[26], "word  ", 6);

  /* display words              */
    p = &outp[32];
    for(i = 0; i < cnt / 2; ++i)
    {   val = ((buf[i * 2 + 1] & 0xff) << 8) + (buf[i * 2] & 0xff);
        sts = find_name(val, &nam);  /* see if name is known                */
        if(0 <= sts)                        /* name is known, use it        */
        {   len = strlen(nam);
            memcpy(p, nam, len);
            p += len;
        }
        else                                /* name is not known, use hex   */
        {   revb[0] = (val >> 8) & 0xff;
            revb[1] = (val     ) & 0xff;
            *p++ = '$';
            if((sts = format_nbytes(revb, 2, p)) < 0)
                return(sts);
            len = 4;
        }
        if(i < cnt / 2 - 1)                 /* comma, unless its last       */
            p[len] = ',';
        p += len + 1;
    }

  /* return                     */
    return(cnt);
}               
               



/***************************************************************
 *  interpret a line of output as 'ascii'
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
int intr_ascii_line(char        *buf,       /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    unsigned    inpcnt,     /* max # bytes              */
                    char        *outp)      /* output buffer            */
               
{   int         i, sts, ch, cnt;
    char        *p;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};
    
    cnt =  inpcnt < 32  ?  inpcnt  :  32;
    memset(outp, ' ', 79);
    outp[79] = '\0';
    
  /* put PC in 1st 4 chars      */
    if((sts = format_2bytes(pc, &outp[0])) < 0)
        return(sts);

  /* put 1st 4 hex bytes in output  */
    if((sts = format_nbytes(&buf[0], cnt < 4 ? cnt : 4, &outp[6])) < 0)
        return(sts);

  /* put op-code in output          */
    memcpy(&outp[26], "ascii ", 6);

  /* display ascii chars            */
    p = &outp[32];
    *p++ = '"';
    for(i = 0; i < cnt; ++i)
    {   ch = buf[i];
        if(ch < ' ' ||  '\x7f' <= ch)
            *p++ = '.';
        else
            *p++ = (char) ch;
    }
    *p++ = '"';

  /* return                     */
    return(cnt);
}               
               



/***************************************************************
 *  interpret a line of output as 'code'
 *
 *  returns # of bytes interpreted for output line
 ***************************************************************/
int intr_code_line( char        *cbuf,      /* buffer to interpret      */
                    unsigned    pc,         /* current PC               */
                    unsigned    inpcnt,     /* max # bytes              */
                    char        *outp)      /* output buffer            */
               
{   int         sts, cnt, opc, ocnt, oclen, shcnt, write;
    int         autoinc, longx, imm_word, prefix;
    unsigned    ofst, dest;
    char        *buf, *p, bitno;
    OPCTBLE     *ope;
    static char *ivopc  = ";**** Invalid Op Code ****";
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};
    
    buf = cbuf;
    memset(outp, ' ', 79);
    outp[79] = '\0';
    oclen = 6;

  /* get op-code entry and set prefix flag  */
    prefix = 0;
    if( (buf[0] & 0xff) == 0x00fe)          /* byte is a prefix code        */
    {   if(opctbl[buf[1] & 0xff].typ & PX)      /* opcode allows prefix     */
        {   prefix = 1;                                 /* set prefix flag  */
            ++buf;                                      /* to next byte     */
        }
    }
    opc = buf[0] & 0xff;
    ope = &opctbl[opc];

    write = ope->typ & WR;

  /* see if there's a long index byte       */
  /* used to add 1 to byte count if present */
    if((ope->typ & 3) == IX)    /* its an indexed instruction       */
        longx = buf[1] & 0x01;          /* get long indexed flag    */
    else                        /* its not an indexed instruction   */
        longx = 0;                      /* no extra bytes           */
    
  /* put PC in 1st 4 chars of output        */
    if((sts = format_2bytes(pc, &outp[0])) < 0)
        return(sts);

  /* put hex bytes in output                */
    cnt = ope->numbyts  +  longx + prefix;
    if((sts = format_nbytes(cbuf, cnt < 4 ? cnt : 4, &outp[6])) < 0)
        return(sts);

  /* put op-code in output                  */
    p = &outp[26];
    if(ope->typ & UK)           /* invalid op code                          */
    {   memcpy(p, "byte  $", 7);
        if((sts = format_nbytes(&buf[0], 1, &outp[33])) < 0)
            return(sts);
        memcpy(&outp[40], ivopc, strlen(ivopc));
    }
    else
        memcpy(p, ope->name, oclen);

    if(prefix)                  /* fix opcode if prefix present             */
    {   int     ocx;
        static  char *names[]   = { "mulu  ", "mul   ", "mulub ", "mulb  ",
                                    "divu  ", "div   ", "divub ", "divb  "  };
        for(ocx = 0; ocx < 4; ocx += 2)
        {   if(!memcmp(ope->name, names[ocx], oclen));
            {   memcpy(p, names[ocx + 1], oclen);
                break;
            }
        }
    }

  /* display operand/s                      */
    ocnt = ope->numops;
    p = &outp[32];

        /******* shifts *********************************/
    if(ope->typ & SH)
    {   shcnt = buf[1] & 0xff;      /* get shift count          */
      /** put register in output            */
        if((sts = format_register(buf[2], 1, p)) < 0)
            return(sts);
        p += sts;
        *p++ = ',';
      /** put shift count in output         */
        if(shcnt < 16)              /* its an immediate value   */
        {   if((sts = format_imm_op(&buf[1], 0, p)) < 0)
                return(sts);
            p    += sts;
        }
        else                        /* its in a register        */
        {   if((sts = format_register(buf[1], 0, p)) < 0)
                return(sts);
            p += sts;
        }
    }

        /******* LJMP and LCALL *************************/
    else if( (opc == 0xe7) || (opc == 0xef) )
    { /** put destination address in output */
        ofst = ((buf[2] << 8) & 0xff00) + (buf[1] & 0xff);
        if(ofst & 0x8000)       /* handle negative  */
            ofst |= ~0x7fff;
        dest = pc + 3 + ofst;   /* destination      */
        if((sts = format_address(dest, p)) < 0)
            return(sts);
        p += sts;
    }

        /******* SJMP and SCALL *************************/
    else if( (opc & 0xf0) == 0x20)
    { /** put destination address in output */
        ofst  = ((opc & 0x7) << 8) + (buf[1] & 0xff);
        if(ofst & 0x0400)       /* handle negative  */
            ofst |= ~0x03ff;
        dest = pc + 2 + ofst;   /* destination      */
        if((sts = format_address(dest, p)) < 0)
            return(sts);
        p += sts;
    }

        /******* conditional jumps **********************/
    else if( 0xd0 <= opc  &&  opc <= 0xdf)
    { /** put destination address in output */
        ofst  = buf[1] & 0xff;
        if(ofst & 0x0080)       /* make negative    */
            ofst |= ~0x00ff;
        dest = pc + 2 + ofst;   /* destination      */
        if((sts = format_address(dest, p)) < 0)
            return(sts);
        p += sts;
    }

        /******* DJNZ ***********************************/
    else if(opc == 0xe0)
    { /** put BREG in output                */
        if((sts = format_register(buf[1], 1, p)) < 0)
            return(sts);
        p += sts;
        *p++ = ',';

      /** put destination address in output */
        ofst  = buf[2];
        if(ofst & 0x80)         /* make negative    */
            ofst |= ~0xff;
        dest = pc + 3 + ofst;   /* destination      */
        if((sts = format_address(dest, p)) < 0)
            return(sts);
        p += sts;
    }

        /******* JBC and JBS    *************************/
    else if((opc & 0xf0) == 0x30)
    { /** put BREG in output                */
        if((sts = format_register(buf[1], 0, p)) < 0)
            return(sts);
        p += sts;
        *p++ = ',';

      /** put bit# in output                */
        bitno = opc & 0x7;
        if((sts = format_bitno(bitno, p)) < 0)
            return(sts);
        p += sts;
        *p++ = ',';

      /** put destination address in output */
        ofst  = buf[2];
        if(ofst & 0x80)         /* make negative    */
            ofst |= ~0xff;
        dest = pc + 3 + ofst;   /* destination      */
        if((sts = format_address(dest, p)) < 0)
            return(sts);
        p += sts;
    }

    else switch(ope->typ & 0x7)
    {
    /******************* direct ******************************/
        case    DI:
            switch(ocnt)
            {   case 1:
                    if((sts = format_register(buf[1], write, p)) < 0)
                        return(sts);
                    p    += sts;
                    break;
                case 2:
                            /**** destination register  ****/
                    if((sts = format_register(buf[2], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source register       ****/
                    if((sts = format_register(buf[1], 0, p)) < 0)
                        return(sts);
                    p += sts;
                    break;
                case 3:
                            /**** destination register  ****/
                    if((sts = format_register(buf[3], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 1 register     ****/
                    if((sts = format_register(buf[2], 0, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 2 register     ****/
                    if((sts = format_register(buf[1], 0, p)) < 0)
                        return(sts);
                    p += sts;
                    break;
            }
            break;

    /******************* IMMEDIATE ***************************/
        case    IM:
            imm_word = (ope->typ & IW) ?  1  :  0;
            switch(ocnt)
            {   case 1:
                    if((sts = format_imm_op(&buf[1], imm_word, p)) < 0)
                        return(sts);
                    p    += sts;
                    break;
                case 2:
                            /**** destination register  ****/
                    if((sts = format_register(buf[2 + imm_word], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source register       ****/
                    if((sts = format_imm_op(&buf[1], imm_word, p)) < 0)
                        return(sts);
                    p += sts;
                    break;
                case 3:
                            /**** destination register  ****/
                    if((sts = format_register(buf[3 + imm_word], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 1 register     ****/
                    if((sts = format_register(buf[2 + imm_word], 0, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 2 register     ****/
                    if((sts = format_imm_op(&buf[1], imm_word, p)) < 0)
                        return(sts);
                    p += sts;
                    break;
            }
            break;

    /******************* INDIRECT ****************************/
        case    ID:
            autoinc = buf[1] & 0x01;        /* auto increment flag  */
            switch(ocnt)
            {   case 1:
                    *p++  = '[';
                    if((sts = format_register(buf[1] & 0xfe, 0, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++ = ']';
                    if(autoinc)
                        *p++ = '+';
                    break;
                case 2:
                            /**** destination register  ****/
                    if((sts = format_register(buf[2], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source register       ****/
                    *p++  = '[';
                    if((sts = format_register(buf[1] & 0xfe, 0, p)) < 0)
                        return(sts);
                    p   += sts;
                    *p++ = ']';
                    if(autoinc)
                        *p++ = '+';
                    break;
                case 3:
                            /**** destination register  ****/
                    if((sts = format_register(buf[3], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 1 register     ****/
                    if((sts = format_register(buf[2], 0, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 2 register     ****/
                    *p++  = '[';
                    if((sts = format_register(buf[1] & 0xfe, 0, p)) < 0)
                        return(sts);
                    p   += sts;
                    *p++ = ']';
                    if(autoinc)
                        *p++ = '+';
                    break;
            }
            break;

    /******************* INDEXED *****************************/
        case    IX:
            switch(ocnt)
            {   case 1:
                    if((sts = format_idx_op(&buf[1], longx, p)) < 0)
                        return(sts);
                    p    += sts;
                    break;
                case 2:
                            /**** destination register  ****/
                    if((sts = format_register(buf[3 + longx], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source register       ****/
                    if((sts = format_idx_op(&buf[1], longx, p)) < 0)
                        return(sts);
                    p  += sts;
                    break;
                case 3:
                            /**** destination register  ****/
                    if((sts = format_register(buf[4 + longx], 1, p)) < 0)
                        return(sts);
                    p    += sts;
                    *p++  = ',';
                            /**** source 1 register     ****/
                    if((sts = format_register(buf[3 + longx], 0, p)) < 0)
                        return(sts);
                    p    += sts;

                    *p++  = ',';
                            /**** source 2 register     ****/
                    if((sts = format_idx_op(&buf[1], longx, p)) < 0)
                        return(sts);
                    p   += sts;
                    break;
            }
            break;
    }

  /* return                     */
    return(cnt);
}



/***************************************************************
 *  format bit # operand to print
 *  output is not a NULL-terminated string
 *
 *  returns:   #bytes put into output if successful
 *      OR    negative error code
 ***************************************************************/
int format_bitno(int    bitno,      /* bit # to format          */
                 char   *outp)      /* output buffer            */
               
{   char        *p;

    if(7 < bitno)
        fprintf(stderr, "**** Internal error, bit #:%d\n", bitno);
    p = outp;
    *p++ = 'B';
    *p++ = 'I';
    *p++ = 'T';
    *p++ = '_';
    *p++ = '0';
    *p++ = bitno + '0';
    return(p - outp);
}



/***************************************************************
 *  format register operand to print
 *  output is not a NULL-terminated string
 *
 *  returns:   #bytes put into output if successful
 *      OR    negative error code
 ***************************************************************/
int format_register(int         reg,        /* register # to format         */
                    int         write,      /* register is being written    */
                    char        *outp)      /* output buffer                */
               
{   int         dig, len, sts;
    char        *p, *nam;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

    p = outp;
/* see if name is known             */
    sts = find_name(reg, &nam);
/* change, if its a write to one of */
/* those different for write        */
    if(0 <= sts)
    {   if(reg == 0x04)
            nam = write ? "AD_Command"  :  "AD_Lo";
        else if(reg == 0x05)
            nam = write ? "Watchdog_Timer"  :  "AD_Hi";
    }
/* name is known, use it            */
    if(0 <= sts)                    
    {   len = strlen(nam);
        memcpy(p, nam, len);
        p += len;
    }
/* name is not known, use hex       */
    else
    {   *p++ = 'R';
        *p++ = 'E';
        *p++ = 'G';
        *p++ = '_';
        dig  = (reg >> 4) & 0xf;        /* get first digit              */
        *p++ = hex[dig];                    /* char. to display         */
        dig  = (reg     ) & 0xf;        /* get 2nd digit                */
        *p++ = hex[dig];                    /* char to display          */
    }
/* return                           */
    return(p - outp);
}



/***************************************************************
 *  format an immediate operand to print
 *  output is not a NULL-terminated string
 *
 *  returns:   #bytes put into output if successful
 *      OR    negative error code
 ***************************************************************/
int format_imm_op(char  *bytep,     /* value to format          */
                  int   word_flg,   /* 0 => byte, 1 => word     */
                  char  *outp)      /* output buffer            */
               
{   int         val, dig;
    char        *p;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

    p    = outp;
    *p++ = '#';
/* get value                        */
    val  = bytep[0] & 0xff;
    if(word_flg)
        val += (bytep[1] << 8) & 0xff00;
/* display value                    */
    *p++ = '$';
    if(word_flg)
    {   dig  = (val >> 12) & 0xf;   /* get first digit of byte      */
        *p++ = hex[dig];                /* char. to display         */
        dig  = (val >>  8) & 0xf;   /* get 2nd digit of byte        */
        *p++ = hex[dig];                /* char to display          */
    }
    dig  = (val >> 4) & 0xf;        /* get first digit of byte      */
    *p++ = hex[dig];                    /* char. to display         */
    dig  = (val     ) & 0xf;        /* get 2nd digit of byte        */
    *p++ = hex[dig];                    /* char to display          */
/* return                           */
    return(p - outp);
}



/***************************************************************
 *  format an indexed operand to print
 *  output is not a NULL-terminated string
 *
 *  returns:   #bytes put into output if successful
 *      OR    negative error code
 ***************************************************************/
int format_idx_op(char  *bytep,     /* ptr to [xreg | ofst ..]  */
                  int   long_flg,   /* 0 => short, 1 => long    */
                  char  *outp)      /* output buffer            */
               
{   int         val, dig, sts;
    char        *p;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

    p    = outp;
/* get value                        */
    val  = bytep[1] & 0xff;
    if(long_flg)
        val += (bytep[2] << 8) & 0xff00;
/* print displacement               */
    *p++ = '$';
    if(long_flg)
    {   dig  = (val >> 12) & 0xf;   /* get first digit of byte      */
        *p++ = hex[dig];                /* char. to display         */
        dig  = (val >>  8) & 0xf;   /* get 2nd digit of byte        */
        *p++ = hex[dig];                /* char to display          */
    }
    dig  = (val >> 4) & 0xf;        /* get first digit of byte      */
    *p++ = hex[dig];                    /* char. to display         */
    dig  = (val     ) & 0xf;        /* get 2nd digit of byte        */
    *p++ = hex[dig];                    /* char to display          */
/* display index register value     */
    *p++ = '[';
    if((sts = format_register(bytep[0] & 0xfe, 0, p)) < 0)
        return(sts);
    p  += sts;
    *p++ = ']';

/* return                           */
    return(p - outp);
}




/***************************************************************
 *  format address to print
 *  output is not a NULL-terminated string
 *
 *  returns:  #bytes put into output if successful
 *      OR    negative error code
 ***************************************************************/
int format_address( unsigned    addr,       /* address to format        */
                    char        *outp)      /* output buffer            */
               
{   int         len, sts;
    char        *p, *nam;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

/* see if name is known                     */
    p = outp;
    sts = find_name(addr, &nam);
/* name is known, use it                    */
    if(0 <= sts)
    {   len = strlen(nam);
        memcpy(p, nam, len);
        p += len;
    }
/* name is not known, use generated label   */
    else
    {   *p++ = 'L';
        len = format_2bytes(addr, p);
        p += len;
    }
/* return                                   */
    return(p - outp);
}



/***************************************************************
 *  format 2 bytes as 4 hex digits to print
 *  output is not a NULL-terminated string
 *
 *  returns:   # characters in output  if successful
 *      OR    negative error code
 ***************************************************************/
int format_2bytes(  unsigned    val,        /* value to format          */
                    char        *outp)      /* output buffer            */
               
{   int         dig;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};
    
    dig  = (val >> 12) & 0xf;       /* get first digit          */
    *outp++ = hex[dig];                 /* put in display       */
    dig  = (val >>  8) & 0xf;       /* get 2nd digit            */
    *outp++ = hex[dig];                 /* put in display       */
    dig  = (val >>  4) & 0xf;       /* get 3rd digit            */
    *outp++ = hex[dig];                 /* put in display       */
    dig  = (val      ) & 0xf;       /* get 4th digit            */
    *outp++ = hex[dig];                 /* put in display       */

    return(4);
}



/***************************************************************
 *  format n bytes as 2*n hex digits to print
 *  output is not a NULL-terminated string
 *
 *  returns:   #bytes in output if successful
 *      OR    negative error code
 ***************************************************************/
int format_nbytes(  char    *val,       /* ptr to value to format   */
                    int     cnt,        /* # of bytes to format     */
                    char    *outp)      /* output buffer            */
               
{   int         i, dig;
    static char hex[16] = { '0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

    for(i = 0; i < cnt; ++i)
    {   dig  = (val[i] >> 4) & 0xf;     /* get first digit              */
        *outp++ = hex[dig];                /* char. to display          */
        dig  = (val[i]     ) & 0xf;     /* get 2nd digit                */
        *outp++ = hex[dig];                /* char to display           */
    }
    return(2 * cnt);
}



/***************************************************************
 *  'process' an instruction for labels
 *
 *  If instruction is a jump of any sort, the bit in the label array is 
 *  set for the destination of the jump. 
 *
 *  returns length of instruction  if successful
 *    OR   negative error code
 ***************************************************************/
int proc_instr_labels(char      *ibuf,      /* buffer containing instr  */
                      unsigned  pc,         /* current PC               */
                      char      labels[])   /* label array to set       */
               
{   int         ilen, opc, ocnt, longx, prefix, jctyp;
    unsigned    ofst, dest;
    char        *buf;
    OPCTBLE     *ope;

    prefix = 0;
    longx  = 0;
    dest   = 0;

  /* set flag for prefix byte if present    */
    buf = ibuf;
    if( (buf[0] & 0xff) == 0x00fe)          /* byte is a prefix code        */
    {   if(opctbl[buf[1] & 0xff].typ & PX)      /* opcode allows prefix     */
        {   prefix = 1;                                 /* set prefix flag  */
            ++buf;                                      /* to next byte     */
        }
    }

  /* get opcode and opcode table entry ptr  */
    opc = buf[0] & 0xff;
    ope = &opctbl[opc];

  /* see if there's a long index byte       */
  /* used to add 1 to byte count if present */
    if((ope->typ & 3) == IX)    /* its an indexed instruction       */
        longx = buf[1] & 0x01;          /* get long indexed flag    */
    else                        /* its not an indexed instruction   */
        longx = 0;                      /* no extra bytes           */

  /* compute length of instruction          */
    ilen = ope->numbyts  +  longx + prefix;

  /* process operand/s                      */
    ocnt = ope->numops;

        /******* SJMP and SCALL   ***********************/
    if( (opc & 0xf0) == 0x20)
    { /** get destination address           */
        ofst  = ((opc & 0x7) << 8) + (buf[1] & 0xff);
        if(ofst & 0x0400)       /* make negative    */
            ofst |= ~0x03ff;
        dest  = pc + 2 + ofst;  /* destination      */
        jctyp = (opc & 0x08) ? JC_CALL : JC_JUMP;
        set_bit(labels, dest);
        add_jcent(dest, pc, jctyp);
    }

        /******* conditional jumps **********************/
    else if( 0xd0 <= opc  &&  opc <= 0xdf)
    { /** get destination address           */
        ofst  = buf[1] & 0xff;
        if(ofst & 0x0080)       /* make negative    */
            ofst |= ~0x00ff;
        dest  = pc + 2 + ofst;  /* destination      */
        set_bit(labels, dest);
        add_jcent(dest, pc, JC_COND);
    }

        /******* DJNZ             ***********************/
    else if(opc == 0xe0)
    { /** get destination address           */
        ofst  = buf[2];
        if(ofst & 0x80)         /* make negative    */
            ofst |= ~0xff;
        dest  = pc + 3 + ofst;  /* destination      */
        set_bit(labels, dest);
        add_jcent(dest, pc, JC_COND);
    }

        /******* JBC and JBS      ***********************/
    else if((opc & 0xf0) == 0x30)
    { /** get destination address           */
        ofst  = buf[2];
        if(ofst & 0x80)         /* handle negative  */
            ofst |= ~0xff;
        dest  = pc + 3 + ofst;  /* destination      */
        set_bit(labels, dest);
        add_jcent(dest, pc, JC_COND);
    }

        /******* LJMP & LCALL     ***********************/
    else if(opc == 0xe7  ||  opc == 0xef)
    {   ofst = ((buf[2] << 8) & 0xff00) + (buf[1] & 0xff);
        if(ofst & 0x8000)       /* handle negative  */
            ofst |= ~0x7fff;
        dest = pc + 3 + ofst;   /* destination      */
        jctyp = (opc == 0xef) ? JC_CALL : JC_JUMP;
        set_bit(labels, dest);
        add_jcent(dest, pc, jctyp);
    }

        /******* BR[]             ***********************/
        /* shouldn't be in 806x's                       */
    else if(opc == 0xe3)
    {
        set_bit(labels, dest);
        add_jcent(dest, pc, JC_COND);
    }

  /* return                     */
    return(ilen);
}
