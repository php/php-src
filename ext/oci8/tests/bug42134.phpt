--TEST--
Bug #42134 (Collection error for invalid collection name)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/details.inc');

// Test collection creation error for normal connection

if (!empty($dbase)) {
    $c = oci_connect($user,$password,$dbase);
}
else {
    $c = oci_connect($user,$password);
}

$collection = oci_new_collection($c, "ABC");
if (!$collection) {
    echo "Normal connection: New Collection error\n";
    $m = oci_error($c);
    var_dump($m);
}

// Test collection creation error for new connection

if (!empty($dbase)) {
    $c = oci_new_connect($user,$password,$dbase);
}
else {
    $c = oci_new_connect($user,$password);
}

$collection = oci_new_collection($c, "DEF");
if (!$collection) {
    echo "New connection: New Collection error\n";
    $m = oci_error($c);
    var_dump($m);
}

// Test collection creation error for persistent connection

if (!empty($dbase)) {
    $c = oci_pconnect($user,$password,$dbase);
}
else {
    $c = oci_pconnect($user,$password);
}

$collection = oci_new_collection($c, "GHI");
if (!$collection) {
    echo "Persistent connection: New Collection error\n";
    $m = oci_error($c);
    var_dump($m);
}

echo "Done\n";

?>
--EXPECTF--
Warning: oci_new_collection(): OCI-22303: type ""."ABC" not found in %s on line %d
Normal connection: New Collection error
array(4) {
  ["code"]=>
  int(22303)
  ["message"]=>
  string(34) "OCI-22303: type ""."ABC" not found"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}

Warning: oci_new_collection(): OCI-22303: type ""."DEF" not found in %s on line %d
New connection: New Collection error
array(4) {
  ["code"]=>
  int(22303)
  ["message"]=>
  string(34) "OCI-22303: type ""."DEF" not found"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}

Warning: oci_new_collection(): OCI-22303: type ""."GHI" not found in %s on line %d
Persistent connection: New Collection error
array(4) {
  ["code"]=>
  int(22303)
  ["message"]=>
  string(34) "OCI-22303: type ""."GHI" not found"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
Done
