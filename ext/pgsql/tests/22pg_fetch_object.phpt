--TEST--
PostgreSQL pg_fetch_object()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

class test_class {
    function __construct($arg1, $arg2) {
        echo __METHOD__ . "($arg1,$arg2)\n";
    }
}

$db = pg_connect($conn_str);

$sql = "SELECT * FROM $table_name WHERE num = 0";
$result = pg_query($db, $sql) or die('Cannot query db');
$rows = pg_num_rows($result);

var_dump(pg_fetch_object($result, NULL, 'test_class', array(1, 2)));

try {
    var_dump(pg_fetch_object($result, NULL, 'does_not_exist'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Ok\n";
?>
--EXPECTF--
test_class::__construct(1,2)
object(test_class)#%d (3) {
  ["num"]=>
  string(1) "0"
  ["str"]=>
  string(3) "ABC"
  ["bin"]=>
  NULL
}
pg_fetch_object(): Argument #3 ($class) must be a valid class name, does_not_exist given
Ok
