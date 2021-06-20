--TEST--
oci_error() error message for parsing error
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs: different error messages from TimesTen
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";

if (!empty($dbase)) {
    var_dump(oci_connect($user, $password, $dbase));
}
else {
    var_dump(oci_connect($user, $password));
}

var_dump($s = oci_parse($c, "WRONG SYNTAX"));
var_dump(oci_execute($s));
var_dump(oci_error($s));

echo "Done\n";

?>
--EXPECTF--
resource(%s) of type (oci8 connection)
resource(%s) of type (oci8 statement)

Warning: oci_execute(): ORA-00900: %s in %s on line %d
bool(false)
array(4) {
  ["code"]=>
  int(900)
  ["message"]=>
  string(%d) "ORA-00900: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(12) "WRONG SYNTAX"
}
Done
