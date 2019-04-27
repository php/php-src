--TEST--
Null bytes in SQL statements
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--INI--
display_errors = On
error_reporting = E_WARNING
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Run Test

echo "Test 1: Valid use of a null byte\n";

$s = oci_parse($c, "select * \0from dual");
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 3: Using a null byte in a bind variable name\n";

$s = oci_parse($c, "select * from dual where :bv = 1");
$bv = 1;
oci_bind_by_name($s, ":bv\0:bv", $bv);
oci_execute($s);


?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1: Valid use of a null byte
array(1) {
  ["DUMMY"]=>
  array(1) {
    [0]=>
    string(1) "X"
  }
}
Test 3: Using a null byte in a bind variable name

Warning: oci_bind_by_name(): ORA-01036: %s in %snull_byte_2.php on line %d

Warning: oci_execute(): ORA-01008: %s in %snull_byte_2.php on line %d
===DONE===
