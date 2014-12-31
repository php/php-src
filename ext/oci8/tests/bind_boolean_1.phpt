--TEST--
Basic PL/SQL "BOOLEAN" (SQLT_BOL) bind test
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die ("skip no oci8 extension"); 
require(dirname(__FILE__).'/connect.inc');
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle Database 12c or greater");
}
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 12)) { 
    die("skip works only with Oracle 12c or greater version of Oracle client libraries");
}
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1\n";

$sql = "begin
        :output1 := true;
        :output2 := false;
       end;";

$s = oci_parse($c, $sql);
oci_bind_by_name($s, ':output1', $output1, -1, OCI_B_BOL);
oci_bind_by_name($s, ':output2', $output2, -1, OCI_B_BOL);
oci_execute($s);
var_dump($output1);
var_dump($output2);

echo "Test 2\n";

$b = "abc";  // bind var type will change
$sql = "begin :b := true; end;";
$s = oci_parse($c, $sql);
oci_bind_by_name($s, ':b', $b, -1, OCI_B_BOL);
oci_execute($s);
var_dump($b);


echo "Test 3\n";

$sql = 
    "begin
    if (:input < 10) then
        :output := true;
    else
        :output := false;
    end if;
end;";
$s = oci_parse($c, $sql);
oci_bind_by_name($s, ':output', $output, -1, OCI_B_BOL);
for ($input = 5; $input < 15; ++$input) {
    oci_bind_by_name($s, ':input', $input);
    oci_execute($s);
    var_dump($output);
}

echo "Test 4\n";

$sql = 
"begin
  if (mod(:userid,2) = 0) then
    :b := true;
  else
    :b := false;
  end if;
end;";
$s = oci_parse($c, $sql);
oci_bind_by_name($s, ':b', $b, -1, OCI_B_BOL);
for ($userid = 1; $userid <= 10; ++$userid) {
    oci_bind_by_name($s, ':userid', $userid, -1, SQLT_INT);
    oci_execute($s);
    var_dump($b);
}

echo "Test 5\n";

$sql = 
"declare
  l boolean;
begin
  l := :b1;
  :b1 := :b2;
  :b2 := l;
end;";
$s = oci_parse($c, $sql);
$b1 = true;
$b2 = false;
var_dump($b1, $b2);
oci_bind_by_name($s, ':b1', $b1, -1, OCI_B_BOL);
oci_bind_by_name($s, ':b2', $b2, -1, OCI_B_BOL);
oci_execute($s);
var_dump($b1, $b2);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
bool(true)
bool(false)
Test 2
bool(true)
Test 3
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Test 4
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
Test 5
bool(true)
bool(false)
bool(false)
bool(true)
===DONE===
