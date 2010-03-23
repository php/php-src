--TEST--
Bug #45458 (OCI8: Numeric keys for associative arrays are not handled properly)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1\n";

$stmt = 'select dummy "a", dummy "20" from dual';

$s = oci_parse($c, $stmt);
oci_execute($s);
$r = oci_fetch_all($s, $data, 0, -1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($data);
var_dump($data[0]);
var_dump($data[0]["a"]);
var_dump($data[0]["20"]);
oci_free_statement($s);

echo "Test 2\n";

$s = oci_parse($c, $stmt);
oci_execute($s);
$r = oci_fetch_all($s, $data, 0, -1, OCI_ASSOC);
var_dump($data);
var_dump($data["a"]);
var_dump($data["20"]);
var_dump($data["a"][0]);
var_dump($data["20"][0]);
oci_free_statement($s);

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
array(1) {
  [0]=>
  array(2) {
    ["a"]=>
    string(1) "X"
    [20]=>
    string(1) "X"
  }
}
array(2) {
  ["a"]=>
  string(1) "X"
  [20]=>
  string(1) "X"
}
string(1) "X"
string(1) "X"
Test 2
array(2) {
  ["a"]=>
  array(1) {
    [0]=>
    string(1) "X"
  }
  [20]=>
  array(1) {
    [0]=>
    string(1) "X"
  }
}
array(1) {
  [0]=>
  string(1) "X"
}
array(1) {
  [0]=>
  string(1) "X"
}
string(1) "X"
string(1) "X"
===DONE===
