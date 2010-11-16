--TEST--
Null bytes in SQL statements
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--INI--
display_errors = On
error_reporting = E_WARNING
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1: Valid use of a null byte\n";

$s = oci_parse($c, "select * \0from dual");
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 2: Invalid use of a null byte\n";

$s = oci_parse($c, "select * from du\0al");
oci_execute($s);

echo "Test 3: Using a null byte in a bind variable name\n";

$s = oci_parse($c, "select * from dual where :bv = 1");
$bv = 1;
oci_bind_by_name($s, ":bv\0:bv", $bv);
oci_execute($s);
 
echo "Test 4: Using a null byte in a bind variable value causing WHERE clause to fail\n";

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
Test 1: Valid use of a null byte
array(1) {
  ["DUMMY"]=>
  array(1) {
    [0]=>
    string(1) "X"
  }
}
Test 2: Invalid use of a null byte

Warning: oci_execute(): ORA-00942: %s in %snull_byte_2.php on line %d
Test 3: Using a null byte in a bind variable name

Warning: oci_bind_by_name(): ORA-01036: %s in %snull_byte_2.php on line %d

Warning: oci_execute(): ORA-01008: %s in %snull_byte_2.php on line %d
Test 4: Using a null byte in a bind variable value causing WHERE clause to fail
array(1) {
  ["DUMMY"]=>
  array(0) {
  }
}
===DONE===
