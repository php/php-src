--TEST--
Bug #74625 (Integer overflow in oci_bind_array_by_name)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$s = oci_parse($c, "BEGIN
      SELECT -1 BULK COLLECT INTO :a FROM DUAL;
  END;");
oci_bind_array_by_name($s, ':a', $a, 5000, 10, SQLT_INT);
oci_execute($s);

var_dump($a);
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Array
(
    [0] => -1
)
===DONE===
