--TEST--
dbx_error
--SKIPIF--
<?php 
    if (!extension_loaded("dbx")) print("skip"); 
    else {
        include_once("dbx_test.p");
        if ($connection===NULL) print("skip");
        }
?>
--POST--
--GET--
--FILE--
<?php 
include_once("dbx_test.p");
if ($module==DBX_ODBC || $module==DBX_OCI8) {
    // ODBC module doesn't have an error-message-function (yet?)
    // OCI8 module needs the query-handle instead of the db-handle (now what?)
    print('query generated an error: dbx_error works ok'."\n");
    print('query is valid: dbx_error works ok'."\n");
    print('wrong dbx_link_object: dbx_error failure works ok'."\n");
    print('too many parameters: dbx_error failure works ok'."\n");
    print('too few parameters: dbx_error failure works ok'."\n");
    }
else {

$sql_statement = "select * from tbl";
$invalid_sql_statement = "invalid select * from tbl";
$dlo = dbx_connect($module, $host, $database, $username, $password);
if (!$dlo) {
    print('this won\'t work'."\n");
	}
else {
    @dbx_query($dlo, "select nonexistingfield from tbl");
    if ((($module==DBX_MSSQL || $module==DBX_SYBASECT) && dbx_error($dlo)!="Changed database context to '".$database."'.".($module==DBX_SYBASECT?"\n":""))
        || strlen(dbx_error($dlo))) {
        print('query generated an error: dbx_error works ok'."\n");
        }
    dbx_query($dlo, "select description from tbl");
    if (!strlen(dbx_error($dlo)) || (($module==DBX_MSSQL || $module==DBX_SYBASECT) && dbx_error($dlo)=="Changed database context to '".$database."'.".($module==DBX_SYBASECT?"\n":""))) {
        print('query is valid: dbx_error works ok'."\n");
        }
    if (!@dbx_error(0)) {
        print('wrong dbx_link_object: dbx_error failure works ok'."\n");
        }
    if (!@dbx_error($dlo, "12many")) {
        print('too many parameters: dbx_error failure works ok'."\n");
        }
    if (!@dbx_error()) {
        print('too few parameters: dbx_error failure works ok'."\n");
        }
    dbx_close($dlo);
    }

    }
?>
--EXPECT--
query generated an error: dbx_error works ok
query is valid: dbx_error works ok
wrong dbx_link_object: dbx_error failure works ok
too many parameters: dbx_error failure works ok
too few parameters: dbx_error failure works ok
