#ifndef GD_FT_TYPES_H
#define GD_FT_TYPES_H

/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Fixed                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    This type is used to store 16.16 fixed-point values, like scaling  */
/*    values or matrix coefficients.                                     */
/*                                                                       */
typedef signed long  GD_FT_Fixed;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Int                                                             */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for the int type.                                        */
/*                                                                       */
typedef signed int  GD_FT_Int;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_UInt                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for the unsigned int type.                               */
/*                                                                       */
typedef unsigned int  GD_FT_UInt;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Long                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for signed long.                                         */
/*                                                                       */
typedef signed long  GD_FT_Long;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_ULong                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for unsigned long.                                       */
/*                                                                       */
typedef unsigned long GD_FT_ULong;

/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Short                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for signed short.                                        */
/*                                                                       */
typedef signed short  GD_FT_Short;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Byte                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A simple typedef for the _unsigned_ char type.                     */
/*                                                                       */
typedef unsigned char  GD_FT_Byte;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Bool                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef of unsigned char, used for simple booleans.  As usual,   */
/*    values 1 and~0 represent true and false, respectively.             */
/*                                                                       */
typedef unsigned char  GD_FT_Bool;



/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Error                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    The FreeType error code type.  A value of~0 is always interpreted  */
/*    as a successful operation.                                         */
/*                                                                       */
typedef int  GD_FT_Error;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    GD_FT_Pos                                                             */
/*                                                                       */
/* <Description>                                                         */
/*    The type GD_FT_Pos is used to store vectorial coordinates.  Depending */
/*    on the context, these can represent distances in integer font      */
/*    units, or 16.16, or 26.6 fixed-point pixel coordinates.            */
/*                                                                       */
typedef signed long  GD_FT_Pos;


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    GD_FT_Vector                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    A simple structure used to store a 2D vector; coordinates are of   */
/*    the GD_FT_Pos type.                                                   */
/*                                                                       */
/* <Fields>                                                              */
/*    x :: The horizontal coordinate.                                    */
/*    y :: The vertical coordinate.                                      */
/*                                                                       */
typedef struct  GD_FT_Vector_
{
  GD_FT_Pos  x;
  GD_FT_Pos  y;

} GD_FT_Vector;


typedef long long int           GD_FT_Int64;
typedef unsigned long long int  GD_FT_UInt64;

typedef signed int              GD_FT_Int32;
typedef unsigned int            GD_FT_UInt32;


#define GD_FT_BOOL( x )  ( (GD_FT_Bool)( x ) )

#define GD_FT_SIZEOF_LONG 4

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif


#endif // GD_FT_TYPES_H
