/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* Written by Sinisa Milivojevic <sinisa@coresinc.com> */

#include <global.h>
#ifdef HAVE_COMPRESS
#include <my_sys.h>
#include <m_string.h>
#include <zlib.h>

/*
** This replaces the packet with a compressed packet
** Returns 1 on error
** *complen is 0 if the packet wasn't compressed
*/

my_bool my_compress(byte *packet, ulong *len, ulong *complen)
{
  if (*len < MIN_COMPRESS_LENGTH)
    *complen=0;
  else
  {
    byte *compbuf=my_compress_alloc(packet,len,complen);
    if (!compbuf)
      return *complen ? 0 : 1;
    memcpy(packet,compbuf,*len);
    my_free(compbuf,MYF(MY_WME));						  }
  return 0;
}


byte *my_compress_alloc(const byte *packet, ulong *len, ulong *complen)
{
  byte *compbuf;
  *complen =  *len * 120 / 100 + 12;
  if (!(compbuf = (byte *) my_malloc(*complen,MYF(MY_WME))))
    return 0;					/* Not enough memory */
  if (compress((Bytef*) compbuf,(ulong *) complen, (Bytef*) packet,
	       (uLong) *len ) != Z_OK)
  {
    my_free(compbuf,MYF(MY_WME));
    return 0;
  }
  if (*complen >= *len)
  {
    *complen=0;
    my_free(compbuf,MYF(MY_WME));
    return 0;
  }
  swap(ulong,*len,*complen);			/* *len is now packet length */
  return compbuf;
}


my_bool my_uncompress (byte *packet, ulong *len, ulong *complen)
{
  if (*complen)					/* If compressed */
  {
    byte *compbuf = (byte *) my_malloc (*complen,MYF(MY_WME));
    if (!compbuf)
      return 1;					/* Not enough memory */
    if (uncompress((Bytef*) compbuf, complen, (Bytef*) packet, *len) != Z_OK)
    {						/* Probably wrong packet */
      my_free (compbuf,MYF(MY_WME));
      return 1;
    }
    *len = *complen;
    memcpy(packet,compbuf,*len);
    my_free(compbuf,MYF(MY_WME));
  }
  return 0;
}
#endif /* HAVE_COMPRESS */
