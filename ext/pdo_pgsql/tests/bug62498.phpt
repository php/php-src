--TEST--
PDO PgSQL Bug #62498 (pdo_pgsql inefficient when getColumnMeta() is used)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
echo "Begin test...\n";

require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

// create the table
$db->exec("CREATE TEMPORARY TABLE bugtest_62498 (intcol INTEGER, stringcol VARCHAR(255), boolcol BOOLEAN, datecol DATE)");

// insert some data
$statement = $db->prepare("INSERT INTO bugtest_62498 (intcol, stringcol, boolcol, datecol) VALUES (:intval, :stringval, :boolval, :dateval)");
$statement->execute(array(
    "intval" => "42",
    "stringval" => "The Answer",
    "boolval" => true,
    "dateval" => '2015-12-14',
));

$select = $db->query('SELECT intcol, stringcol, boolcol, datecol FROM bugtest_62498');
$meta = [];
for ($i=0; $i < 4; $i++) {
  $meta[] = $select->getColumnMeta(0);
}
var_dump($meta);

?>
Done
--EXPECT--
Begin test...
array(4) {
  [0]=>
  array(6) {
    ["pgsql:oid"]=>
    int(23)
    ["native_type"]=>
    string(4) "int4"
    ["name"]=>
    string(6) "intcol"
    ["len"]=>
    int(4)
    ["precision"]=>
    int(-1)
    ["pdo_type"]=>
    int(1)
  }
  [1]=>
  array(6) {
    ["pgsql:oid"]=>
    int(23)
    ["native_type"]=>
    string(4) "int4"
    ["name"]=>
    string(6) "intcol"
    ["len"]=>
    int(4)
    ["precision"]=>
    int(-1)
    ["pdo_type"]=>
    int(1)
  }
  [2]=>
  array(6) {
    ["pgsql:oid"]=>
    int(23)
    ["native_type"]=>
    string(4) "int4"
    ["name"]=>
    string(6) "intcol"
    ["len"]=>
    int(4)
    ["precision"]=>
    int(-1)
    ["pdo_type"]=>
    int(1)
  }
  [3]=>
  array(6) {
    ["pgsql:oid"]=>
    int(23)
    ["native_type"]=>
    string(4) "int4"
    ["name"]=>
    string(6) "intcol"
    ["len"]=>
    int(4)
    ["precision"]=>
    int(-1)
    ["pdo_type"]=>
    int(1)
  }
}
Done
