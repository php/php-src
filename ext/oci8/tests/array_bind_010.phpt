--TEST--
oci_bind_array_by_name() and invalid values 8
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$statement = oci_parse($c, 'SELECT user FROM all_objects');

$array = Array(1,2,3,4,5);

oci_bind_array_by_name($statement, ":c1", $array, 5, 5, SQLT_CHR);

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_bind_array_by_name(): ORA-01036: illegal variable name/number in %s on line %d
array(5) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "4"
  [4]=>
  string(1) "5"
}
Done
