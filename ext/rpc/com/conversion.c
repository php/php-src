/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#ifdef PHP_WIN32

#include "php.h"
#include "php_COM.h"
#include "unknwn.h"

// prototypes

PHPAPI void php_pval_to_variant(pval *pval_arg, VARIANT *var_arg, int codepage);
PHPAPI void php_pval_to_variant_ex(pval *pval_arg, VARIANT *var_arg, pval *pval_type, int codepage);
PHPAPI void php_variant_to_pval(VARIANT *var_arg, pval *pval_arg, int persistent, int codepage);
PHPAPI OLECHAR *php_char_to_OLECHAR(char *C_str, uint strlen, int codepage);
PHPAPI char *php_OLECHAR_to_char(OLECHAR *unicode_str, uint *out_length, int persistent, int codepage);

// implementations

PHPAPI void php_pval_to_variant(pval *pval_arg, VARIANT *var_arg, int codepage)
{
   OLECHAR *unicode_str;

   switch (pval_arg->type)
   {
      case IS_NULL:
         var_arg->vt = VT_EMPTY;
         break;

      case IS_BOOL:
         var_arg->vt = VT_BOOL;
         var_arg->boolVal = (short)pval_arg->value.lval;
         break;

      case IS_OBJECT:
         if(!strcmp(pval_arg->value.obj.ce->name, "VARIANT"))
         {
            int type;
            pval **var_handle;

            /* fetch the VARIANT structure */
            zend_hash_index_find(pval_arg->value.obj.properties, 0, (void **) &var_handle);
            var_arg->pvarVal = (VARIANT FAR*) zend_list_find((*var_handle)->value.lval, &type);
            var_arg->vt = VT_VARIANT|VT_BYREF;
         }
         else if(!strcmp(pval_arg->value.obj.ce->name, "COM"))
         {
            pval **idispatch_handle;
            i_dispatch *obj;
            int type;

            /* fetch the IDispatch interface */
            zend_hash_index_find(pval_arg->value.obj.properties, 0, (void **) &idispatch_handle);
            obj = (i_dispatch *)zend_list_find((*idispatch_handle)->value.lval, &type);
            if (!obj || (type != php_COM_get_le_idispatch()))
               var_arg->vt = VT_EMPTY;
            else
            {
				var_arg->vt = VT_DISPATCH;
				var_arg->pdispVal = obj->i.dispatch;
            }
         }
         else
            var_arg->vt = VT_EMPTY;
         break;

      case IS_RESOURCE:
      case IS_CONSTANT:
      case IS_CONSTANT_ARRAY:
      case IS_ARRAY:
         var_arg->vt = VT_EMPTY;
         break;

      case IS_LONG:
         var_arg->vt = VT_I4;   /* assuming 32-bit platform */
         var_arg->lVal = pval_arg->value.lval;
         break;

      case IS_DOUBLE:
         var_arg->vt = VT_R8;   /* assuming 64-bit double precision */
         var_arg->dblVal = pval_arg->value.dval;
         break;

      case IS_STRING:
         var_arg->vt = VT_BSTR;
         unicode_str = php_char_to_OLECHAR(pval_arg->value.str.val, pval_arg->value.str.len, codepage);
         var_arg->bstrVal = SysAllocString(unicode_str);
         efree(unicode_str);
   }
}

PHPAPI void php_pval_to_variant_ex(pval *pval_arg, VARIANT *var_arg, pval *pval_type, int codepage)
{
      OLECHAR *unicode_str;

      var_arg->vt = (short) pval_type->value.lval;

      switch(var_arg->vt)
      {
         case VT_UI1:
            convert_to_long_ex(&pval_arg);
            var_arg->bVal = (unsigned char)pval_arg->value.lval;
            break;

         case VT_I2:
            convert_to_long_ex(&pval_arg);
            var_arg->iVal = (short)pval_arg->value.lval;
            break;

         case VT_I4:
            convert_to_long_ex(&pval_arg);
            var_arg->lVal = pval_arg->value.lval;
            break;

         case VT_R4:
            convert_to_double_ex(&pval_arg);
            var_arg->fltVal = (float)pval_arg->value.dval;
            break;

         case VT_R8:
            convert_to_double_ex(&pval_arg);
            var_arg->dblVal = pval_arg->value.dval;
            break;

         case VT_BOOL:
            convert_to_boolean_ex(&pval_arg);
            var_arg->boolVal = (short)pval_arg->value.lval;
            break;

         case VT_ERROR:
            convert_to_long_ex(&pval_arg);
            var_arg->scode = pval_arg->value.lval;
            break;

         case VT_CY:
            convert_to_double_ex(&pval_arg);
            VarCyFromR8(pval_arg->value.dval, &(var_arg->cyVal));
            break;

         case VT_DATE:
         {
            SYSTEMTIME wintime;
            struct tm *phptime;

            phptime = gmtime(&(pval_arg->value.lval));

            wintime.wYear = phptime->tm_year + 1900;
            wintime.wMonth = phptime->tm_mon + 1;
            wintime.wDay = phptime->tm_mday;
            wintime.wHour = phptime->tm_hour;
            wintime.wMinute = phptime->tm_min;
            wintime.wSecond = phptime->tm_sec;

            SystemTimeToVariantTime(&wintime, &(var_arg->date));
         }

         case VT_BSTR:
            convert_to_string_ex(&pval_arg);
            unicode_str = php_char_to_OLECHAR(pval_arg->value.str.val, pval_arg->value.str.len, codepage);
            var_arg->bstrVal = SysAllocString(unicode_str);
            efree(unicode_str);
            break;

         case VT_DECIMAL:
            convert_to_string_ex(&pval_arg);
            unicode_str = php_char_to_OLECHAR(pval_arg->value.str.val, pval_arg->value.str.len, codepage);
            VarDecFromStr(unicode_str, LOCALE_SYSTEM_DEFAULT, 0, &(var_arg->decVal));
            break;

         case VT_DECIMAL|VT_BYREF:
            convert_to_string_ex(&pval_arg);
            unicode_str = php_char_to_OLECHAR(pval_arg->value.str.val, pval_arg->value.str.len, codepage);
            VarDecFromStr(unicode_str, LOCALE_SYSTEM_DEFAULT, 0, var_arg->pdecVal);
            break;

         case VT_UNKNOWN:
            php_pval_to_variant(pval_arg, var_arg, codepage);
            if(var_arg->vt != VT_DISPATCH)
               var_arg->vt = VT_EMPTY;
            else
            {
               HRESULT hr;

               hr = var_arg->pdispVal->lpVtbl->QueryInterface(var_arg->pdispVal, &IID_IUnknown, &(var_arg->punkVal));

               if (FAILED(hr))
               {
                  php_error(E_WARNING,"can't query IUnknown");
                  var_arg->vt = VT_EMPTY;
               }
               else
                  var_arg->vt = VT_UNKNOWN;
            }
            break;

         case VT_DISPATCH:
            php_pval_to_variant(pval_arg, var_arg, codepage);
            if(var_arg->vt != VT_DISPATCH)
               var_arg->vt = VT_EMPTY;
            break;

         case VT_UI1|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->pbVal = (unsigned char FAR*) &(pval_arg->value.lval);
            break;

         case VT_I2|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->piVal = (short FAR*) &(pval_arg->value.lval);
            break;

         case VT_I4|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->plVal = (long FAR*) &(pval_arg->value.lval);
            break;

         case VT_R4|VT_BYREF:
            convert_to_double(pval_arg);
            var_arg->pfltVal = (float FAR*) &(pval_arg->value.lval);
            break;

         case VT_R8|VT_BYREF:
            convert_to_double(pval_arg);
            var_arg->pdblVal = (double FAR*) &(pval_arg->value.lval);
            break;

         case VT_BOOL|VT_BYREF:
            convert_to_boolean(pval_arg);
            var_arg->pboolVal = (short FAR*) &(pval_arg->value.lval);
            break;

         case VT_ERROR|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->pscode = (long FAR*) &(pval_arg->value.lval);
            break;

         case VT_CY|VT_BYREF:
            convert_to_double_ex(&pval_arg);
            VarCyFromR8(pval_arg->value.dval, var_arg->pcyVal);
            break;

         case VT_DATE|VT_BYREF:
         {
            SYSTEMTIME wintime;
            struct tm *phptime;

            phptime = gmtime(&(pval_arg->value.lval));

            wintime.wYear   = phptime->tm_year + 1900;
            wintime.wMonth  = phptime->tm_mon + 1;
            wintime.wDay    = phptime->tm_mday;
            wintime.wHour   = phptime->tm_hour;
            wintime.wMinute = phptime->tm_min;
            wintime.wSecond = phptime->tm_sec;

            SystemTimeToVariantTime(&wintime, var_arg->pdate);
         }

         case VT_BSTR|VT_BYREF:
            convert_to_string(pval_arg);
            var_arg->pbstrVal = (BSTR FAR*) emalloc(sizeof(BSTR FAR*));
            unicode_str = php_char_to_OLECHAR(pval_arg->value.str.val, pval_arg->value.str.len, codepage);
            *(var_arg->pbstrVal) = SysAllocString(unicode_str);
            efree(unicode_str);
            break;

         case VT_UNKNOWN|VT_BYREF:
            php_pval_to_variant(pval_arg, var_arg, codepage);
            if(var_arg->vt != VT_DISPATCH)
               var_arg->vt = VT_EMPTY;
            else
            {
               HRESULT hr;

               hr = var_arg->pdispVal->lpVtbl->QueryInterface(var_arg->pdispVal, &IID_IUnknown, &(var_arg->punkVal));

               if (FAILED(hr))
               {
                  php_error(E_WARNING,"can't query IUnknown");
                  var_arg->vt = VT_EMPTY;
               }
               else
                  var_arg->vt = VT_UNKNOWN|VT_BYREF;
            }
            break;

         case VT_DISPATCH|VT_BYREF:
            php_pval_to_variant(pval_arg, var_arg, codepage);
            if(var_arg->vt != VT_DISPATCH)
               var_arg->vt = VT_EMPTY;
            else
               var_arg->vt |= VT_BYREF;
            break;

         case VT_VARIANT|VT_BYREF:
            php_pval_to_variant(pval_arg, var_arg, codepage);
            if(var_arg->vt != (VT_VARIANT | VT_BYREF))
               var_arg->vt = VT_EMPTY;
            break;

         case VT_I1:
            convert_to_long_ex(&pval_arg);
            var_arg->cVal = (char)pval_arg->value.lval;
            break;

         case VT_UI2:
            convert_to_long_ex(&pval_arg);
            var_arg->uiVal = (unsigned short)pval_arg->value.lval;
            break;

         case VT_UI4:
            convert_to_long_ex(&pval_arg);
            var_arg->ulVal = (unsigned long)pval_arg->value.lval;
            break;

         case VT_INT:
            convert_to_long_ex(&pval_arg);
            var_arg->intVal = (int)pval_arg->value.lval;
            break;

         case VT_UINT:
            convert_to_long_ex(&pval_arg);
            var_arg->uintVal = (unsigned int)pval_arg->value.lval;
            break;

         case VT_I1|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->pcVal = (char FAR*) &pval_arg->value.lval;
            break;

         case VT_UI2|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->puiVal = (unsigned short FAR*) &pval_arg->value.lval;
            break;

         case VT_UI4|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->pulVal = (unsigned long FAR*) &pval_arg->value.lval;
            break;

         case VT_INT|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->pintVal = (int FAR*) &pval_arg->value.lval;
            break;

         case VT_UINT|VT_BYREF:
            convert_to_long(pval_arg);
            var_arg->puintVal = (unsigned int FAR*) &pval_arg->value.lval;
            break;

         default:
            php_error(E_WARNING, "Type not supportet or not yet implemented.");
      }
}

PHPAPI void php_variant_to_pval(VARIANT *var_arg, pval *pval_arg, int persistent, int codepage)
{

   switch(var_arg->vt & ~VT_BYREF)
   {
      case VT_EMPTY:
         var_uninit(pval_arg);
         break;

      case VT_UI1:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long)*(var_arg->pbVal);
         else
            pval_arg->value.lval = (long) var_arg->bVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_I2:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long )*(var_arg->piVal);
         else
            pval_arg->value.lval = (long) var_arg->iVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_I4:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = *(var_arg->plVal);
         else
            pval_arg->value.lval = var_arg->lVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_R4:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.dval = (double)*(var_arg->pfltVal);
         else
            pval_arg->value.dval = (double) var_arg->fltVal;

         pval_arg->type = IS_DOUBLE;
         break;

      case VT_R8:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.dval = *(var_arg->pdblVal);
         else
            pval_arg->value.dval = var_arg->dblVal;

         pval_arg->type = IS_DOUBLE;
         break;

      case VT_DECIMAL:
         {
            OLECHAR *unicode_str;
            switch(VarBstrFromDec(&var_arg->decVal, LOCALE_SYSTEM_DEFAULT, 0, &unicode_str))
            {
               case S_OK:
                  pval_arg->value.str.val = php_OLECHAR_to_char(unicode_str, &pval_arg->value.str.len, persistent, codepage);
                  pval_arg->type = IS_STRING;
                  break;

               default:
                  php_error(E_WARNING, "Error converting DECIMAL value to PHP floating point");
                  break;
            }
         }
         break;

      case VT_CY:
         if(var_arg->vt & VT_BYREF)
		    VarR8FromCy(var_arg->cyVal, &(pval_arg->value.dval));
         else
		    VarR8FromCy(*(var_arg->pcyVal), &(pval_arg->value.dval));

         pval_arg->type = IS_DOUBLE;
         break;

      case VT_BOOL:
         if (var_arg->vt & VT_BYREF)
            if (*(var_arg->pboolVal) & 0xFFFF)
               pval_arg->value.lval = 1;
            else
               pval_arg->value.lval = 0;
         else
            if (var_arg->boolVal & 0xFFFF)
               pval_arg->value.lval = 1;
            else
               pval_arg->value.lval = 0;

         pval_arg->type = IS_BOOL;
         break;

      case VT_NULL:
      case VT_VOID:
         pval_arg->type = IS_NULL;
         break;

      case VT_VARIANT:
         php_variant_to_pval(var_arg->pvarVal, pval_arg, persistent, codepage);
         break;

      case VT_BSTR:
         if (pval_arg->is_ref == 0  || (var_arg->vt & VT_BYREF) != VT_BYREF) {
            pval_arg->value.str.val = php_OLECHAR_to_char(var_arg->bstrVal, &pval_arg->value.str.len, persistent, codepage);
            SysFreeString(var_arg->bstrVal);
         } else {
            pval_arg->value.str.val = php_OLECHAR_to_char(*(var_arg->pbstrVal), &pval_arg->value.str.len, persistent, codepage);
            SysFreeString(*(var_arg->pbstrVal));
            efree(var_arg->pbstrVal);
         }

         pval_arg->type = IS_STRING;
         break;

      case VT_DATE: {
            SYSTEMTIME wintime;
            struct tm phptime;

            VariantTimeToSystemTime(var_arg->date, &wintime);

            phptime.tm_year  = wintime.wYear - 1900;
            phptime.tm_mon   = wintime.wMonth - 1;
            phptime.tm_mday  = wintime.wDay;
            phptime.tm_hour  = wintime.wHour;
            phptime.tm_min   = wintime.wMinute;
            phptime.tm_sec   = wintime.wSecond;
            phptime.tm_isdst = -1;

            tzset();
            pval_arg->value.lval = mktime(&phptime);
            pval_arg->type = IS_LONG;
         }
         break;
      case VT_DISPATCH: {
            pval *handle;
			i_dispatch *obj;

            pval_arg->type=IS_OBJECT;
            pval_arg->value.obj.ce = &com_class_entry;
            pval_arg->value.obj.properties = (HashTable *) emalloc(sizeof(HashTable));
            pval_arg->is_ref=1;
            pval_arg->refcount=1;
            zend_hash_init(pval_arg->value.obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

            ALLOC_ZVAL(handle);
			obj = emalloc(sizeof(i_dispatch));
			php_COM_set(obj, var_arg->pdispVal, TRUE);

            handle->type = IS_LONG;
			handle->value.lval = zend_list_insert(obj, php_COM_get_le_idispatch());
            pval_copy_constructor(handle);
            INIT_PZVAL(handle);
            zend_hash_index_update(pval_arg->value.obj.properties, 0, &handle, sizeof(pval *), NULL);
         }
         break;
      case VT_UNKNOWN:
//wtf ??
		  var_arg->pdispVal->lpVtbl->Release(var_arg->pdispVal);
         /* break missing intentionally */

      case VT_I1:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long)*(var_arg->pcVal);
         else
            pval_arg->value.lval = (long) var_arg->cVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_UI2:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long)*(var_arg->puiVal);
         else
            pval_arg->value.lval = (long) var_arg->uiVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_UI4:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long)*(var_arg->pulVal);
         else
            pval_arg->value.lval = (long) var_arg->ulVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_INT:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long)*(var_arg->pintVal);
         else
            pval_arg->value.lval = (long) var_arg->intVal;

         pval_arg->type = IS_LONG;
         break;

      case VT_UINT:
         if(var_arg->vt & VT_BYREF)
            pval_arg->value.lval = (long)*(var_arg->puintVal);
         else
            pval_arg->value.lval = (long) var_arg->uintVal;

         pval_arg->type = IS_LONG;
         break;

      default:
         php_error(E_WARNING,"Unsupported variant type: %d (0x%X)", var_arg->vt, var_arg->vt);
         var_reset(pval_arg);
         break;
   }
}

PHPAPI OLECHAR *php_char_to_OLECHAR(char *C_str, uint strlen, int codepage)
{
   OLECHAR *unicode_str;

   //request needed buffersize
   uint reqSize = MultiByteToWideChar(codepage, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, C_str, -1, NULL, 0);

   if(reqSize)
   {
      unicode_str = (OLECHAR *) emalloc(sizeof(OLECHAR) * reqSize);

      //convert string
      MultiByteToWideChar(codepage, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, C_str, -1, unicode_str, reqSize);
   }
   else
   {
      unicode_str = (OLECHAR *) emalloc(sizeof(OLECHAR));
      *unicode_str = 0;

      switch(GetLastError())
      {
         case ERROR_NO_UNICODE_TRANSLATION:
            php_error(E_WARNING,"No unicode translation available for the specified string");
            break;
         default:
            php_error(E_WARNING,"Error in php_char_to_OLECHAR()");
      }
   }

   return unicode_str;
}

PHPAPI char *php_OLECHAR_to_char(OLECHAR *unicode_str, uint *out_length, int persistent, int codepage)
{
   char *C_str;
   uint length = 0;

   //request needed buffersize
   uint reqSize = WideCharToMultiByte(codepage, WC_COMPOSITECHECK, unicode_str, -1, NULL, 0, NULL, NULL);

   if(reqSize)
   {
      C_str = (char *) pemalloc(sizeof(char) * reqSize, persistent);

      //convert string
      length = WideCharToMultiByte(codepage, WC_COMPOSITECHECK, unicode_str, -1, C_str, reqSize, NULL, NULL) - 1;
   }
   else
   {
      C_str = (char *) pemalloc(sizeof(char), persistent);
      *C_str = 0;

      php_error(E_WARNING,"Error in php_OLECHAR_to_char()");
   }

   if(out_length)
      *out_length = length;

   return C_str;
}

#endif /* PHP_WIN32 */