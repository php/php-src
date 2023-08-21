--TEST--
PECL Bug #16842 (NO_DATA_FOUND exception is a warning)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--INI--
error_reporting = E_WARNING
--FILE--
<?php

require __DIR__.'/connect.inc';

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
--EXPECTF--
Test 1
Raises NO_DATA_FOUND

Warning: oci_execute(): ORA-01403: %s
ORA-06512: at line %d in %specl_bug16842.php on line %d
bool(false)
array(4) {
  ["code"]=>
  int(1403)
  ["message"]=>
  string(%d) "ORA-01403: %s
ORA-06512: at line 1"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(31) "begin raise NO_DATA_FOUND; end;"
}
Test 2
Raises ZERO_DIVIDE

Warning: oci_execute(): ORA-01476: %s
ORA-06512: at line 1 in %s on line 19
bool(false)
array(4) {
  ["code"]=>
  int(1476)
  ["message"]=>
  string(%d) "ORA-01476: %s
ORA-06512: at line 1"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(29) "begin raise ZERO_DIVIDE; end;"
}
