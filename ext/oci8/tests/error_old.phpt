--TEST--
ocierror()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

if (!empty($dbase)) {
	var_dump(ocilogon($user, $password, $dbase));
}
else {
	var_dump(ocilogon($user, $password));
}

var_dump($s = ociparse($c, "WRONG SYNTAX"));
var_dump(ociexecute($s));
var_dump(ocierror($s));

echo "Done\n";

?>
--EXPECTF--
resource(%s) of type (oci8 connection)
resource(%s) of type (oci8 statement)

Warning: ociexecute(): ORA-00900: invalid SQL statement in %s on line %d
bool(false)
array(4) {
  ["code"]=>
  int(900)
  ["message"]=>
  string(32) "ORA-00900: invalid SQL statement"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(12) "WRONG SYNTAX"
}
Done
