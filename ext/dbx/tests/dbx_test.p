<?php
// set the $connection to match your test-database setup
// (possibly change the rest of the settings too)
// $fieldname_case_function was introduced for oci8, as oracle returns
//    the fieldnames in uppercase
// e.g. $connection = DBX_MYSQL;
$connection = NULL;
switch ($connection) {
    case DBX_MYSQL:
        $module=DBX_MYSQL;
        $host="";
        $database="dbx_test";
        $username="dbx_testuser";
        $password="dbx_testpassword";
        $module_name="mysql";
        $fieldname_case_function="dbx_unchangedcase";
        break;
    case DBX_MSSQL:
        $module=DBX_MSSQL;
        $host="HOLLY";
        $database="dbx_test";
        $username="";
        $password="";
        $module_name="mssql";
        $fieldname_case_function="dbx_unchangedcase";
        break;
    case DBX_ODBC:
        $module=DBX_ODBC;
        $host="localhost";
        $database="dbx_test_mysql"; // "dbx_test_access";
        $username="dbx_testuser";
        $password="dbx_testpassword";
        $module_name="odbc";
        $fieldname_case_function="dbx_unchangedcase";
        break;
    case DBX_PGSQL:
        $module=DBX_PGSQL;
        $host="";
        $database="dbx_test";
        $username="dbx_testuser";
        $password="dbx_testpassword";
        $module_name="pgsql";
        $fieldname_case_function="dbx_unchangedcase";
        break;
    case DBX_FBSQL:
        $module=DBX_FBSQL;
        $host="";
        $database="dbx_test";
        $username="dbx_testuser";
        $password="dbx_testpassword";
        $module_name="fbsql";
        $fieldname_case_function="dbx_unchangedcase";
        break;
    case DBX_OCI8:
        $module=DBX_OCI8;
        $host="";
        $database="dbx_test";
        $username="dbx_testuser";
        $password="dbx_testpassword";
        $module_name="oci8";
        $fieldname_case_function="dbx_uppercase";
        break;
    case DBX_SYBASECT:
        $module=DBX_SYBASECT;
        $host="threesyb";
        $database="dbx_test";
        $username="dbx_testuser";
        $password="dbx_testpassword";
        $module_name="sybase_ct";
        $fieldname_case_function="dbx_unchangedcase";
        break;
    }

function dbx_uppercase($sz) {
    return strtoupper($sz);
    }
function dbx_unchangedcase($sz) {
    return $sz;
    }

?>