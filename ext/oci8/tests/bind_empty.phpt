--TEST--
binding empty values
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$drop = "DROP table bind_empty_tab";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table bind_empty_tab(name VARCHAR(10))";
$statement = oci_parse($c, $create);
oci_execute($statement);


echo "Test 1\n";

$name = null;
$stmt = oci_parse($c, "UPDATE bind_empty_tab SET name=:name");
oci_bind_by_name($stmt, ":name", $name);

var_dump(oci_execute($stmt));

echo "Test 2\n";

$name = "";
$stmt = oci_parse($c, "UPDATE bind_empty_tab SET name=:name");
oci_bind_by_name($stmt, ":name", $name);

var_dump(oci_execute($stmt));

echo "Test 3\n";

$stmt = oci_parse($c, "INSERT INTO bind_empty_tab (NAME) VALUES ('abc')");
$res = oci_execute($stmt);

$stmt = oci_parse($c, "INSERT INTO bind_empty_tab (NAME) VALUES ('def')");
$res = oci_execute($stmt);

$name = null;
$stmt = oci_parse($c, "UPDATE bind_empty_tab SET name=:name WHERE NAME = 'abc'");
oci_bind_by_name($stmt, ":name", $name);

var_dump(oci_execute($stmt));

$stid = oci_parse($c, "select * from bind_empty_tab order by 1");
oci_execute($stid);
oci_fetch_all($stid, $res);
var_dump($res);

echo "Test 4\n";

$name = "";
$stmt = oci_parse($c, "UPDATE bind_empty_tab SET name=:name WHERE NAME = 'def'");
oci_bind_by_name($stmt, ":name", $name);

var_dump(oci_execute($stmt));

$stid = oci_parse($c, "select * from bind_empty_tab order by 1");
oci_execute($stid);
oci_fetch_all($stid, $res);
var_dump($res);

echo "Test 5\n";

$av = $bv = 'old';
$s = oci_parse($c, "begin :bv := null; end; ");
oci_bind_by_name($s, ":bv", $bv);
oci_execute($s);
var_dump($av);
var_dump($bv);

echo "Test 6\n";

$av = $bv = null;
$s = oci_parse($c, "begin :bv := null; end; ");
oci_bind_by_name($s, ":bv", $bv);
oci_execute($s);
var_dump($av);
var_dump($bv);

// Clean up

$drop = "DROP table bind_empty_tab";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
bool(true)
Test 2
bool(true)
Test 3
bool(true)
array(1) {
  ["NAME"]=>
  array(2) {
    [0]=>
    string(3) "def"
    [1]=>
    NULL
  }
}
Test 4
bool(true)
array(1) {
  ["NAME"]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
}
Test 5
string(3) "old"
NULL
Test 6
NULL
NULL
===DONE===
