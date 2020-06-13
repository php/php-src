--TEST--
Bug #51291 (oci_error() doesn't report last error when called two times)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

echo "Test 1 - Parse\n";

$s = @oci_parse($c, "select ' from dual");
if (!$s) {
    var_dump(oci_error($c));
    echo "2nd call\n";
    var_dump(oci_error($c));
}

echo "\nTest 2 - Parse\n";

$s = @oci_parse($c, "select ' from dual");
if (!$s) {
    var_dump(oci_error(), oci_error($c));
    echo "2nd call\n";
    var_dump(oci_error(), oci_error($c));
}

echo "\nTest 3 - Execute\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error($s));
    echo "2nd call\n";
    var_dump(oci_error($s));
}

echo "\nTest 4 - Execute - consecutive oci_error calls of different kinds\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    echo "2nd call\n";
    var_dump(oci_error(), oci_error($c), oci_error($s));
}


echo "\nTest 5 - Execute - after oci_rollback\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    $r = oci_rollback($c);
    echo "Rollback status is ";
    if (is_null($r)) echo "null";
    else if ($r === false) echo "false";
    else if ($r === true) echo "true";
    else echo $r;
    echo "\n";
    echo "2nd call after oci_rollback\n";
    var_dump(oci_error(), oci_error($c), oci_error($s));
}


echo "\nTest 6 - Execute - after successful 2nd query with new handle\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    $s2 = oci_parse($c, 'select 1 from dual');
    $r = oci_execute($s2, OCI_DEFAULT);
    echo "Execute status is ";
    if (is_null($r)) echo "null";
    else if ($r === false) echo "false";
    else if ($r === true) echo "true";
    else echo $r;
    echo "\n";
    echo "2nd call after successful execute\n";
    var_dump(oci_error(), oci_error($c), oci_error($s), oci_error($s2));
}


echo "\nTest 7 - Execute - after successful 2nd query with same handle\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    $s = oci_parse($c, 'select 1 from dual');
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


echo "\nTest 8 - Execute - after unsuccessful 2nd query with new handle\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    $s2 = oci_parse($c, 'select reallynothere from dual');
    $r = oci_execute($s2, OCI_DEFAULT);
    echo "Execute status is ";
    if (is_null($r)) echo "null";
    else if ($r === false) echo "false";
    else if ($r === true) echo "true";
    else echo $r;
    echo "\n";
    echo "2nd call after unsuccessful execute\n";
    var_dump(oci_error(), oci_error($c), oci_error($s), oci_error($s2));
}

echo "\nTest 9 - Execute - after unsuccessful 2nd query with same handle\n";

$s = @oci_parse($c, 'select doesnotexist from dual');
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    var_dump(oci_error(), oci_error($c), oci_error($s));
    $s = oci_parse($c, 'select reallynothere from dual');
    $r = oci_execute($s, OCI_DEFAULT);
    echo "Execute status is ";
    if (is_null($r)) echo "null";
    else if ($r === false) echo "false";
    else if ($r === true) echo "true";
    else echo $r;
    echo "\n";
    echo "2nd call after unsuccessful execute\n";
    var_dump(oci_error(), oci_error($c), oci_error($s));
}

?>
--EXPECTF--
Test 1 - Parse
array(4) {
  ["code"]=>
  int(1756)
  ["message"]=>
  string(48) "ORA-01756: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
2nd call
array(4) {
  ["code"]=>
  int(1756)
  ["message"]=>
  string(48) "ORA-01756: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}

Test 2 - Parse
bool(false)
array(4) {
  ["code"]=>
  int(1756)
  ["message"]=>
  string(48) "ORA-01756: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
2nd call
bool(false)
array(4) {
  ["code"]=>
  int(1756)
  ["message"]=>
  string(48) "ORA-01756: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}

Test 3 - Execute
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
2nd call
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}

Test 4 - Execute - consecutive oci_error calls of different kinds
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
2nd call
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}

Test 5 - Execute - after oci_rollback
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
Rollback status is true
2nd call after oci_rollback
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}

Test 6 - Execute - after successful 2nd query with new handle
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
Execute status is true
2nd call after successful execute
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
bool(false)

Test 7 - Execute - after successful 2nd query with same handle
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
Execute status is true
2nd call after successful execute
bool(false)
bool(false)
bool(false)

Test 8 - Execute - after unsuccessful 2nd query with new handle
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}

Warning: oci_execute(): ORA-00904: %sREALLYNOTHERE%s in %sbug51291_1.php on line %d
Execute status is false
2nd call after unsuccessful execute
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904%sREALLYNOTHERE%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(30) "select reallynothere from dual"
}

Test 9 - Execute - after unsuccessful 2nd query with same handle
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904:%sDOESNOTEXIST%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(29) "select doesnotexist from dual"
}

Warning: oci_execute(): ORA-00904: %sREALLYNOTHERE%s in %sbug51291_1.php on line %d
Execute status is false
2nd call after unsuccessful execute
bool(false)
bool(false)
array(4) {
  ["code"]=>
  int(904)
  ["message"]=>
  string(%d) "ORA-00904%sREALLYNOTHERE%s"
  ["offset"]=>
  int(%d)
  ["sqltext"]=>
  string(30) "select reallynothere from dual"
}
