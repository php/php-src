--TEST--
Null bytes in SQL statements
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--INI--
display_errors = On
error_reporting = E_WARNING
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Run Test

echo "Test 1: Invalid use of a null byte\n";

$s = oci_parse($c, "select * from du\0al");
oci_execute($s);

echo "Test 2: Using a null byte in a bind variable value causing WHERE clause to fail\n";

$s = oci_parse($c, "select * from dual where :bv = 'abc'");
$bv = 'abc\0abc';
oci_bind_by_name($s, ":bv", $bv);
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1: Invalid use of a null byte

Warning: oci_execute(): ORA-00942: %s in %snull_byte_3.php on line %d
Test 2: Using a null byte in a bind variable value causing WHERE clause to fail
array(1) {
  ["DUMMY"]=>
  array(0) {
  }
}
===DONE===
