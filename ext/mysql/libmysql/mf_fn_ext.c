/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB
   This file is public domain and comes with NO WARRANTY of any kind */

/* Returnerar en pekare till filnamnets extension. */

#include "mysys_priv.h"
#include <m_string.h>

	/* Returnera en pekare till ett filnamns extension.
	   Pekaren pekar p} extensions-karakt{ren. Om ingen extension hittas
	   returneras en pekare till NULL-tecknet i filnamnet */
	/* Denna funktion r|r inte p} utg}ngsnamnet */

my_string fn_ext(const char *name)
{
  register my_string pos,gpos;
  DBUG_ENTER("fn_ext");
  DBUG_PRINT("mfunkt",("name: '%s'",name));

#if defined(FN_DEVCHAR) || defined(FN_C_AFTER_DIR)
  {
    char buff[FN_REFLEN];
    gpos=(my_string) name+dirname_part(buff,(char*) name);
  }
#else
  if (!(gpos=strrchr(name,FNLIBCHAR)))
    gpos=name;
#endif
  pos=strrchr(gpos,FN_EXTCHAR);
  DBUG_RETURN (pos ? pos : strend(gpos));
} /* fn_ext */
