--TEST--
PECL Bug #16842 (NO_DATA_FOUND exception is a warning)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--INI--
error_reporting = E_WARNING
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1\n";

echo "Raises NO_DATA_FOUND\n";
$s = oci_parse($c, 'begin raise NO_DATA_FOUND; end;');
$e = oci_execute($s);
var_dump($e);
var_dump(oci_error($s));

echo "Test 2\n";

echo "Raises ZERO_DIVIDE\n";
$s = oci_parse($c, 'begin raise ZERO_DIVIDE; end;');
$e = oci_execute($s);
var_dump($e);
var_dump(oci_error($s));

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
Raises NO_DATA_FOUND

Warning: oci_execute(): OCI_NO_DATA in %s on line 11
bool(false)
array(4) {
  [%u|b%"code"]=>
  int(1403)
  [%u|b%"message"]=>
  %unicode|string%(45) "ORA-01403: %s
ORA-06512: at line 1"
  [%u|b%"offset"]=>
  int(0)
  [%u|b%"sqltext"]=>
  %unicode|string%(31) "begin raise NO_DATA_FOUND; end;"
}
Test 2
Raises ZERO_DIVIDE

Warning: oci_execute(): ORA-01476: %s
ORA-06512: at line 1 in %s on line 19
bool(false)
array(4) {
  [%u|b%"code"]=>
  int(1476)
  [%u|b%"message"]=>
  %unicode|string%(56) "ORA-01476: %s
ORA-06512: at line 1"
  [%u|b%"offset"]=>
  int(0)
  [%u|b%"sqltext"]=>
  %unicode|string%(29) "begin raise ZERO_DIVIDE; end;"
}
===DONE===
