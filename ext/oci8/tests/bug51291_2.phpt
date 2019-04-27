--TEST--
Bug #51291 (oci_error() doesn't report last error when called two times)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs: different error messages from TimesTen
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

echo "\nTest 1 - Execute - after successful 2nd query with same statement\n";

$s = oci_parse($c, "declare e exception; begin if :bv = 1 then raise e; end if; end;");
$bv = 1;
oci_bind_by_name($s, ":bv", $bv);
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    $bv = 0;
    $r = oci_execute($s, OCI_DEFAULT);
    echo "Execute status is ";
    if (is_null($r)) echo "null";
    else if ($r === false) echo "false";
    else if ($r === true) echo "true";
    else echo $r;
    echo "\n";
    echo "2nd call after successful execute\n";
    var_dump(oci_error(), oci_error($c), oci_error($s));
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 - Execute - after successful 2nd query with same statement
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(6510)
  ["message"]=>
  string(72) "ORA-06510: PL/SQL: %s
ORA-06512: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(64) "declare e exception; begin if :bv = 1 then raise e; end if; end;"
}
Execute status is true
2nd call after successful execute
bool(false)
bool(false)
bool(false)
===DONE===
