--TEST--
Bug #47199 (pg_delete fails on NULL)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
require_once('inc/skipif.inc');
?>
--FILE--
<?php

require_once('inc/config.inc');

$dbh = pg_connect($conn_str);
$tbl_name = 'test_47199';
@pg_query($dbh, "DROP TABLE $tbl_name");
pg_query($dbh, "CREATE TABLE $tbl_name (null_field INT, not_null_field INT NOT NULL)");

pg_insert($dbh, $tbl_name, array('null_field' => null, 'not_null_field' => 1));
pg_insert($dbh, $tbl_name, array('null_field' => null, 'not_null_field' => 2));

var_dump(pg_fetch_all(pg_query($dbh, 'SELECT * FROM '. $tbl_name)));

$query = pg_delete($dbh, $tbl_name, array('null_field' => NULL,'not_null_field' => 2), PGSQL_DML_STRING|PGSQL_DML_EXEC);

echo $query, "\n";

$query = pg_update($dbh, $tbl_name, array('null_field' => NULL, 'not_null_field' => 0), array('not_null_field' => 1, 'null_field' => ''), PGSQL_DML_STRING|PGSQL_DML_EXEC);

echo $query, "\n";

var_dump(pg_fetch_all(pg_query($dbh, 'SELECT * FROM '. $tbl_name)));

@pg_query($dbh, "DROP TABLE $tbl_name");
pg_close($dbh);

echo PHP_EOL."Done".PHP_EOL;

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    ["null_field"]=>
    NULL
    ["not_null_field"]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["null_field"]=>
    NULL
    ["not_null_field"]=>
    string(1) "2"
  }
}
DELETE FROM "test_47199" WHERE "null_field" IS NULL AND "not_null_field"=2;
UPDATE "test_47199" SET "null_field"=NULL,"not_null_field"=0 WHERE "not_null_field"=1 AND "null_field" IS NULL;
array(1) {
  [0]=>
  array(2) {
    ["null_field"]=>
    NULL
    ["not_null_field"]=>
    string(1) "0"
  }
}

Done
