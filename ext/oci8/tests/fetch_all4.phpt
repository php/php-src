--TEST--
Test oci_fetch_* array overwriting when query returns no rows
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table fetch_all4_tab",
	"create table fetch_all4_tab (mycol1 number, mycol2 varchar2(20))",
	"insert into fetch_all4_tab values (1, 'abc')"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
			    942 // table or view does not exist
			,  2289 // sequence does not exist
			,  4080 // trigger does not exist
                        , 38802 // edition does not exist
                ))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "select * from fetch_all4_tab where 1 = 0");
oci_execute($s);
$res = array(1,2,3);  // this array is replaced as a result of the query
$r = oci_fetch_all($s, $res);
var_dump($r);
var_dump($res);

echo "Test 2\n";

$s = oci_parse($c, "select * from fetch_all4_tab where 1 = 0");
oci_execute($s);
$row = array(1,2,3);  // this array is replaced as a result of the query
$row = oci_fetch_array($s);
var_dump($row);

// Clean up

$stmtarray = array(
	"drop table fetch_all4_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
int(0)
array(2) {
  [%u|b%"MYCOL1"]=>
  array(0) {
  }
  [%u|b%"MYCOL2"]=>
  array(0) {
  }
}
Test 2
bool(false)
===DONE===
