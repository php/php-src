--TEST--
dbx_escape_string
--SKIPIF--
<?php 
include_once("skipif.inc");
?>
--FILE--
<?php 
include_once("dbx_test.p");
$sql_select_statement = "select * from tbl where id=999999 and parentid=999999";
$sql_insert_statement = "insert into tbl (id, parentid, description) values (999999, 999999, 'temporary_record')";
$sql_update_statement = "update tbl set field2 = '%s' where id=999999 and parentid=999999";
$sql_delete_statement = "delete from tbl where id=999999 and parentid=999999";
$dlo = dbx_connect($module, $host, $database, $username, $password);
if (!$dlo) {
    print('this won\'t work'."\n");
	}
else {
    // insert query
    dbx_query($dlo, $sql_insert_statement);
    // dbx_escape_string should make sure that an escaped string
    // is neatly passed through to the database, and on readout
    // it should be returned exactly the same as it went in
    function test_dbx_escape_string($txt) 
    {
        $sql = sprintf($GLOBALS['sql_update_statement'], dbx_escape_string($GLOBALS['dlo'], $txt));
        if (dbx_query($GLOBALS['dlo'], $sql)) {
            if ($dro=dbx_query($GLOBALS['dlo'], $GLOBALS['sql_select_statement'])) {
                print(($dro->data[0]['field2']===$txt?'ok':'fail ('.$dro->data[0]['field2'].')').': '.$txt."\n");
                }
            }
    }
    test_dbx_escape_string('no special characters');
    test_dbx_escape_string('quote \' string');
    test_dbx_escape_string('doublequote " string');
    test_dbx_escape_string('backslash \\ string');
    test_dbx_escape_string('backslash and quote \\ \' string');
    test_dbx_escape_string('ampersand & string');
    // delete query
    dbx_query($dlo, $sql_delete_statement);
    // generate errors
    if (!@dbx_escape_string(0, "any_text")) {
        print('wrong dbx_link_object: escape_string failure works ok'."\n");
        }
    if (!@dbx_escape_string($dlo, "any_text", "12many")) {
        print('too many parameters: escape_string failure works ok'."\n");
        }
    if (!@dbx_escape_string($dlo)) {
        print('too few parameters: escape_string failure works ok'."\n");
        }
    dbx_close($dlo);
    }
?>
--EXPECT--
ok: no special characters
ok: quote ' string
ok: doublequote " string
ok: backslash \ string
ok: backslash and quote \ ' string
ok: ampersand & string
wrong dbx_link_object: escape_string failure works ok
too many parameters: escape_string failure works ok
too few parameters: escape_string failure works ok
