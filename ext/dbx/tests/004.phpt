--TEST--
dbx_close
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
$dlo = dbx_connect($module, $host, $database, $username, $password);
if ($module===DBX_OCI8) { // close for oci8 always return NULL since it doesn't do anything
    print('close works ok'."\n");
    }
else {
    if ($dlo!=0) {
        if (dbx_close($dlo)) {
            print('close works ok'."\n");
            }
        }
    }
if (!@dbx_close($dlo)) {
    print('close failure works ok'."\n");
    }
if (!@dbx_close($dlo, "12many")) {
    print('too many parameters: close failure works ok'."\n");
    }
if (!@dbx_close()) {
    print('too few parameters: close failure works ok'."\n");
    }
?>
--EXPECT--
close works ok
close failure works ok
too many parameters: close failure works ok
too few parameters: close failure works ok