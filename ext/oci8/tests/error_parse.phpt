--TEST--
Test error handling when persistent connection is passed to oci_error()
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

// As part of the fix for Bug 42134, an error handling difference was
// noticed when oci_error() was passed a persistent connection.  This
// was fixed and the behavior of oci_error() for all connections types
// was made consistent.

require __DIR__.'/details.inc';

// Test parse error for normal connection

if (!empty($dbase)) {
    $c1 = oci_connect($user,$password,$dbase);
}
else {
    $c1 = oci_connect($user,$password);
}

$s = @oci_parse($c1, "select ' from dual");
if (!$s) {
    echo "Normal connection: Parse error\n";
    $m = oci_error($c1);
    var_dump($m);
}

// Test parse error for new connection

if (!empty($dbase)) {
    $c2 = oci_new_connect($user,$password,$dbase);
}
else {
    $c2 = oci_new_connect($user,$password);
}

$s = @oci_parse($c2, "select ' from dual");
if (!$s) {
    echo "New connection: Parse error\n";
    $m = oci_error($c2);
    var_dump($m);
}

// Test parse error for persistent connection

if (!empty($dbase)) {
    $c3 = oci_pconnect($user,$password,$dbase);
}
else {
    $c3 = oci_pconnect($user,$password);
}

$s = @oci_parse($c3, "select ' from dual");
if (!$s) {
    echo "Persistent connection: Parse error\n";
    $m = oci_error($c3);
    var_dump($m);
}

// Verify that passing no connection doesn't affect future calls

$m = oci_error();
echo "No connection: error: ";
var_dump($m);

// Check the errors are still accessible in the respective handles

$m = oci_error($c1);
echo "Normal connection (take #2): Parse error: ";
echo $m["message"], "\n";

$m = oci_error($c2);
echo "New connection (take #2): Parse error: ";
echo $m["message"], "\n";

$m = oci_error($c3);
echo "Persistent connection (take #2): Parse error: ";
echo $m["message"], "\n";

// Now create a new error for a normal connection and check all again

$s = @oci_new_collection($c1, "ABC");
$m = oci_error($c1);
echo "Normal connection: New Collection error: ";
echo $m["message"], "\n";

$m = oci_error($c2);
echo "New connection (take #3): Parse error: ";
echo $m["message"], "\n";

$m = oci_error($c3);
echo "Persistent connection (take #3): Parse error: ";
echo $m["message"], "\n";

echo "Done\n";

?>
--EXPECTF--
Normal connection: Parse error
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
New connection: Parse error
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
Persistent connection: Parse error
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
No connection: error: bool(false)
Normal connection (take #2): Parse error: ORA-01756: %s
New connection (take #2): Parse error: ORA-01756: %s
Persistent connection (take #2): Parse error: ORA-01756: %s
Normal connection: New Collection error: OCI-22303: type ""."ABC" not found
New connection (take #3): Parse error: ORA-01756: %s
Persistent connection (take #3): Parse error: ORA-01756: %s
Done
