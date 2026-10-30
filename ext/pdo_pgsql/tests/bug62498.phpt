--TEST--
PDO PgSQL Bug #62498 (pdo_pgsql inefficient when getColumnMeta() is used), 64-bit
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
if (PHP_INT_SIZE < 8) die("skip valid for 64-bit only");
?>
--FILE--
<?php
echo "Begin test...\n";

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

// create the table
$db->exec("CREATE TEMPORARY TABLE test62498 (int2col INT2, int4col INT4, int8col INT8, stringcol VARCHAR(255), boolcol BOOLEAN, datecol DATE, textcol TEXT, tscol TIMESTAMP, byteacol BYTEA)");

// insert some data
$statement = $db->prepare("INSERT INTO test62498 (int2col, int4col, int8col, stringcol, boolcol, datecol, textcol, tscol, byteacol) VALUES (:int2val, :int4val, :int8val, :stringval, :boolval, :dateval, :textval, :tsval, :byteaval)");
$vals = array(
    "int2val" => "42",
    "int4val" => "42",
    "int8val" => "42",
    "stringval" => "The Answer",
    "boolval" => true,
    "dateval" => '2015-12-14',
    "textval" => "some text",
    "tsval"   => 19990108,
    "byteaval" => 0,
);
$statement->execute($vals);

$select = $db->query('SELECT int2col, int4col, int8col, stringcol, boolcol, datecol, textcol, tscol, byteacol FROM test62498');
$meta = [];
for ($i=0; $i < count($vals); $i++) {
  $meta[] = $select->getColumnMeta($i);
}
var_dump($meta);

?>
Done
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS test62498');
--EXPECTF--
Begin test...
array(9) {
  [0]=>
  array(8) {
    ["pgsql:oid"]=>
    int(21)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(4) "int2"
    ["pdo_type"]=>
    int(1)
    ["name"]=>
    string(7) "int2col"
    ["len"]=>
    int(2)
    ["precision"]=>
    int(-1)
  }
  [1]=>
  array(8) {
    ["pgsql:oid"]=>
    int(23)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(4) "int4"
    ["pdo_type"]=>
    int(1)
    ["name"]=>
    string(7) "int4col"
    ["len"]=>
    int(4)
    ["precision"]=>
    int(-1)
  }
  [2]=>
  array(8) {
    ["pgsql:oid"]=>
    int(20)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(4) "int8"
    ["pdo_type"]=>
    int(1)
    ["name"]=>
    string(7) "int8col"
    ["len"]=>
    int(8)
    ["precision"]=>
    int(-1)
  }
  [3]=>
  array(8) {
    ["pgsql:oid"]=>
    int(1043)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(7) "varchar"
    ["pdo_type"]=>
    int(2)
    ["name"]=>
    string(9) "stringcol"
    ["len"]=>
    int(-1)
    ["precision"]=>
    int(259)
  }
  [4]=>
  array(8) {
    ["pgsql:oid"]=>
    int(16)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(4) "bool"
    ["pdo_type"]=>
    int(5)
    ["name"]=>
    string(7) "boolcol"
    ["len"]=>
    int(1)
    ["precision"]=>
    int(-1)
  }
  [5]=>
  array(8) {
    ["pgsql:oid"]=>
    int(1082)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(4) "date"
    ["pdo_type"]=>
    int(2)
    ["name"]=>
    string(7) "datecol"
    ["len"]=>
    int(4)
    ["precision"]=>
    int(-1)
  }
  [6]=>
  array(8) {
    ["pgsql:oid"]=>
    int(25)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(4) "text"
    ["pdo_type"]=>
    int(2)
    ["name"]=>
    string(7) "textcol"
    ["len"]=>
    int(-1)
    ["precision"]=>
    int(-1)
  }
  [7]=>
  array(8) {
    ["pgsql:oid"]=>
    int(1114)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(9) "timestamp"
    ["pdo_type"]=>
    int(2)
    ["name"]=>
    string(5) "tscol"
    ["len"]=>
    int(8)
    ["precision"]=>
    int(-1)
  }
  [8]=>
  array(8) {
    ["pgsql:oid"]=>
    int(17)
    ["pgsql:table_oid"]=>
    int(%d)
    ["table"]=>
    string(%s) "test62498"
    ["native_type"]=>
    string(5) "bytea"
    ["pdo_type"]=>
    int(3)
    ["name"]=>
    string(8) "byteacol"
    ["len"]=>
    int(-1)
    ["precision"]=>
    int(-1)
  }
}
Done
