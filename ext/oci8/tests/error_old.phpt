--TEST--
ocierror()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs: different error messages from TimesTen
require(dirname(__FILE__).'/skipif.inc');
?> 
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

Warning: ociexecute(): ORA-00900: %s in %s on line %d
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
