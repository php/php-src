--TEST--
dbx_query
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
$sql_statement = "select * from tbl order by id";
$invalid_sql_statement = "invalid select * from tbl";
$sql_select_statement = "select * from tbl where id=999999 and parentid=999999";
$sql_insert_statement = "insert into tbl (id, parentid, description) values (999999, 999999, 'temporary_record')";
$sql_update_statement = "update tbl set field2 = 'bla''bla\"bla' where id=999999 and parentid=999999";
$sql_delete_statement = "delete from tbl where id=999999 and parentid=999999";
$dlo = dbx_connect($module, $host, $database, $username, $password);
if (!$dlo) {
    print('this won\'t work'."\n");
	}
else {
    // especially for sybase I need to set the textsize to >64 k, as one of the test-fields 
    // requires this (shouldn't this be a php.ini-entry??)
    if ($connection === DBX_SYBASECT) @dbx_query($dlo, "set textsize 100000");
    // select query
    if ($dro=dbx_query($dlo, $sql_statement)) {
        for ($i=0; $i<$dro->rows; ++$i) {
            print($dro->data[$i][$fieldname_case_function('id')].".".$dro->data[$i][$fieldname_case_function('description')].".".$dro->data[$i][$fieldname_case_function('field1')].".".strlen($dro->data[$i][$fieldname_case_function('field2')])."\n");
            }
        $dro->data[0][$fieldname_case_function('id')]='changed_value';
        print($dro->data[0][0]."\n");
        }
    // insert query
    if (dbx_query($dlo, $sql_insert_statement)) {
        print('insert-query: dbx_query works ok'."\n");
        if ($dro=dbx_query($dlo, $sql_select_statement)) {
            for ($i=0; $i<$dro->rows; ++$i) {
                print($dro->data[$i][$fieldname_case_function('id')].".".$dro->data[$i][$fieldname_case_function('description')].".".strlen($dro->data[$i][$fieldname_case_function('field2')])."\n");
                }
            }
        }
    // update query
    if (dbx_query($dlo, $sql_update_statement)) {
        print('update-query: dbx_query works ok'."\n");
        if ($dro=dbx_query($dlo, $sql_select_statement)) {
            for ($i=0; $i<$dro->rows; ++$i) {
                print($dro->data[$i][$fieldname_case_function('id')].".".$dro->data[$i][$fieldname_case_function('description')].".".strlen($dro->data[$i][$fieldname_case_function('field2')])."\n");
                }
            }
        }
    // delete query
    if (dbx_query($dlo, $sql_delete_statement)) {
        print('delete-query: dbx_query works ok'."\n");
        if ($dro=dbx_query($dlo, $sql_select_statement)) {
            for ($i=0; $i<$dro->rows; ++$i) {
                print($dro->data[$i][$fieldname_case_function('id')].".".$dro->data[$i][$fieldname_case_function('description')].".".strlen($dro->data[$i][$fieldname_case_function('field2')])."\n");
                }
            }
        }

    // generate errors
    if (!@dbx_query(0, $sql_statement)) {
        print('wrong dbx_link_object: query failure works ok'."\n");
        }
    if (!@dbx_query($dlo, $invalid_sql_statement)) {
        print('wrong sql-statement: query failure works ok'."\n");
        }
    if (!@dbx_query($dlo, $sql_statement, DBX_RESULT_INDEX, "12many")) {
        print('too many parameters: query failure works ok'."\n");
        }
    if (!@dbx_query($dlo)) {
        print('too few parameters: query failure works ok'."\n");
        }
    dbx_close($dlo);
    }
?>
--EXPECT--
1.root.empty fields.0
10.abc.field2 contains single quote.3
20.cba.field2 contains double quote.3
30.bac.field2 contains >4k text.4591
40.100.field2 contains >64k text.70051
50.20.empty fields.0
60.20.empty fields.0
changed_value
insert-query: dbx_query works ok
999999.temporary_record.0
update-query: dbx_query works ok
999999.temporary_record.11
delete-query: dbx_query works ok
wrong dbx_link_object: query failure works ok
wrong sql-statement: query failure works ok
too many parameters: query failure works ok
too few parameters: query failure works ok
