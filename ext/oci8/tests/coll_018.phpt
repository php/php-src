--TEST--
Collection trim tests
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__."/create_type.inc";

$coll1 = oci_new_collection($c, $type_name);

echo "\nTest 2.\n";
var_dump($coll1->trim(0));

echo "\nTest 3.\n";
var_dump($coll1->append(1));
var_dump($coll1->append(2));
var_dump($coll1->append(3));
var_dump($coll1->append(4));

var_dump($coll1->getElem(-1));  // check before the beginning
var_dump($coll1->getElem(0));
var_dump($coll1->getElem(1));
var_dump($coll1->getElem(2));
var_dump($coll1->getElem(3));
var_dump($coll1->getElem(4));  // check past the end

echo "\nTest 4.\n";
var_dump($coll1->trim(1));
var_dump($coll1->getElem(2));  // this should be the last element
var_dump($coll1->getElem(3));  // this element should have gone

echo "\nTest 5.\n";
var_dump($coll1->trim(2));
var_dump($coll1->getElem(0));  // this should be the last element
var_dump($coll1->getElem(1));  // this element should have gone

echo "\nTest 6.\n";
var_dump($coll1->trim(0));
var_dump($coll1->getElem(0));  // this should still be the last element

echo "\nTest 7.\n";
var_dump($coll1->trim(1));
var_dump($coll1->getElem(0));  // this should have gone

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECT--
Test 2.
bool(true)

Test 3.
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
float(1)
float(2)
float(3)
float(4)
bool(false)

Test 4.
bool(true)
float(3)
bool(false)

Test 5.
bool(true)
float(1)
bool(false)

Test 6.
bool(true)
float(1)

Test 7.
bool(true)
bool(false)
Done
