--TEST--
DBX_CONSTANTS
--SKIPIF--
<?php if (!extension_loaded("dbx")) print("skip"); ?>
--FILE--
<?php 
if (DBX_MYSQL=="DBX_MYSQL")               print('!DBX_MYSQL');
if (DBX_ODBC=="DBX_ODBC")                 print('!DBX_ODBC');
if (DBX_PGSQL=="DBX_PGSQL")               print('!DBX_PGSQL');
if (DBX_MSSQL=="DBX_MSSQL")               print('!DBX_MSSQL');
if (DBX_FBSQL=="DBX_FBSQL")               print('!DBX_FBSQL');
if (DBX_OCI8=="DBX_OCI8")                 print('!DBX_OCI8');
if (DBX_SYBASECT=="DBX_SYBASECT")         print('!DBX_SYBASECT');
if (DBX_PERSISTENT=="DBX_PERSISTENT")     print('!DBX_PERSISTENT');
if (DBX_RESULT_INFO=="DBX_RESULT_INFO")   print('!DBX_RESULT_INFO');
if (DBX_RESULT_INDEX=="DBX_RESULT_INDEX") print('!DBX_RESULT_INDEX');
if (DBX_RESULT_ASSOC=="DBX_RESULT_ASSOC") print('!DBX_RESULT_ASSOC');
if (DBX_CMP_NATIVE=="DBX_CMP_NATIVE")     print('!DBX_CMP_NATIVE');
if (DBX_CMP_TEXT=="DBX_CMP_TEXT")         print('!DBX_CMP_TEXT');
if (DBX_CMP_NUMBER=="DBX_CMP_NUMBER")     print('!DBX_CMP_NUMBER');
if (DBX_CMP_ASC=="DBX_CMP_ASC")           print('!DBX_CMP_ASC');
if (DBX_CMP_DESC=="DBX_CMP_DESC")         print('!DBX_CMP_DESC');
print('ok');
?>
--EXPECT--
ok