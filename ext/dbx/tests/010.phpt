--TEST--
dbx_fetch_row
--SKIPIF--
<?php 
include_once("skipif.inc");
?>
--INI--
magic_quotes_runtime=0
--FILE--
<?php 
include_once("dbx_test.p");
$sql_statement = "select * from tbl order by id";

$dlo = dbx_connect($module, $host, $database, $username, $password);
if (!$dlo) {
    print('this won\'t work'."\n");
	}
else {
    // especially for sybase I need to set the textsize to >64 k, as one of the test-fields 
    // requires this (shouldn't this be a php.ini-entry??)
    if ($connection === DBX_SYBASECT) @dbx_query($dlo, "set textsize 100000");
    // select query
    if ($dro=dbx_query($dlo, $sql_statement, DBX_RESULT_UNBUFFERED)) {
        while ($row = dbx_fetch_row($dro)) {
            print($dro->rows.".".$row['id'].".".$row['description'].".".$row['field1'].".".strlen($row['field2'])."\n");
            }
        }
    // generate errors
    $invalid_result_object = 'invalid';
    if (!@dbx_fetch_row($invalid_result_object)) {
        print('wrong dbx_result_object: fetch_row failure works ok'."\n");
        }
    if (!@dbx_fetch_row($dro, "12many")) {
        print('too many parameters: fetch_row failure works ok'."\n");
        }
    if (!@dbx_fetch_row()) {
        print('too few parameters: fetch_row failure works ok'."\n");
        }
    dbx_close($dlo);
    }
?>
--EXPECT--
1.1.root.empty fields.0
2.10.abc.field2 contains single quote.3
3.20.cba.field2 contains double quote.3
4.30.bac.field2 contains >4k text.4591
5.40.100.field2 contains >64k text.70051
6.50.20.empty fields.0
7.60.20.empty fields.0
wrong dbx_result_object: fetch_row failure works ok
too many parameters: fetch_row failure works ok
too few parameters: fetch_row failure works ok
