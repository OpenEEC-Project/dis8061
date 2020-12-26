/*****************************************************************************
 *   symbol table routines
 *
 *   source file -->   sym.c
 *
 *  Copyright   1997,1998   William Lawrance
 *****************************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>


#define MAXSYM  2000


typedef struct
{   unsigned    symval;         /* value of symbol                  */
    char        symnam[34];     /* name in ASCII (NULL-terminated)  */
}   SYMTBE;

SYMTBE  *symtab = {NULL};       /* pointer to symbol table          */
SYMTBE  *nxtsym = {NULL};       /* next availabe entry              */
SYMTBE  *endsym = {NULL};       /* end of symbol table              */

SYMTBE  *currsym    = {NULL};   /* current entry for 'next_name'    */



/************************************************
 *   init symbol table search by PC
 *
 *   returns:
 *     0 if successfull
 *       OR negative error code
 ************************************************/
int     init_name_by_pc(void)

{   currsym = symtab;

        /**** sort by PC here       ****/

    return(0);
}




/************************************************
 *   get next name for a PC
 *
 *   returns:
 *     0 if successfull
 *       OR negative error code
 ************************************************/
int     next_name(  unsigned    pc,         /* PC to get name for       */
                    char        **name)     /* returned ptr to name     */

{   
/* if no symbol table entries; return, no symbols to use            */
    if(!symtab)
        return(-1);
/* PC is less than current symbol; return, no symbols to use        */
    if(pc < currsym->symval)
        return(-1);
/* PC is greater than current symbol; advance current table ptr     */
    while(currsym->symval < pc  &&  currsym < endsym)
        ++currsym;
/* return a name, if the PC is equal                                */
    if(pc == currsym->symval)           /* entry found          */
    {   *name = currsym->symnam;            /* return name ptr  */
        ++currsym;                          /* incr table ptr   */
        return(0);
    }
/* none found - return                                              */
    return(-1);
}



/************************************************
 *   find name for an address
 *
 *   returns:
 *     >= 0 if successfull, 'fnam' is valid
 *     <  0 if unsuccessful -- addr was not found in symbol table
 ************************************************/
int     find_name(addr, fnam)
unsigned    addr;               /* address to search for                     */
char        **fnam;             /* returned name                             */

{   SYMTBE      *sp;
    
    *fnam = NULL;
    if(symtab == NULL)                      /* no symbol table present      */
        return(-1);

    for(sp = symtab; sp < nxtsym; ++sp)
    {   if(sp->symval == addr)
        {   *fnam = sp->symnam;
            return(0);
        }
    }
    return(-1);
}



/************************************************
 *   find addr for a name
 *
 *   returns addr corresponding to name supplied
 *   returns NULL is name is unknown
 ************************************************/
int     *find_addr(fnam)
char    *fnam;              /* name to search for           */

{   SYMTBE      *sp;

    if(symtab == NULL)                      /* no symbol table present  */
        return(NULL);
    for(sp = symtab; sp < endsym ;++sp)
        if(!strcmp(fnam, sp->symnam))       /* found it                 */
            return(&sp->symval);
    return(NULL);
}



/************************************************
 *   add name to table
 *
 *   returns    0 if successful
 *       OR    <0 if error
 ************************************************/
int     add_name(fnam, addr)
char        *fnam;              /* name to add                  */
unsigned    addr;               /* address to assoc. with name  */

{   if(symtab == NULL)          /* no symbol table present - alloc it       */
    {   if((symtab = (SYMTBE *) malloc(MAXSYM * sizeof(SYMTBE))) == NULL)
            return(-10);
        nxtsym = symtab;
        endsym = symtab + MAXSYM;
    }
    if(nxtsym == endsym)                        /* table is full            */
        return(-11);
    if(sizeof(symtab->symnam) <= strlen(fnam))  /* name is too long         */
        return(-12);
    nxtsym->symval = addr;                      /* put address in table     */
    strcpy(nxtsym->symnam, fnam);               /* put name in table        */
    ++nxtsym;                                   /* incr next ptr            */
    return(0);
}

