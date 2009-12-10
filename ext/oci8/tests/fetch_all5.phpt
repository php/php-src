--TEST--
Test oci_fetch_all with 0 and -1 skip & maxrows
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table fetch_all5_tab",
	"create table fetch_all5_tab (mycol1 number, mycol2 varchar2(20))",
	"insert into fetch_all5_tab values (1, 'abc')",
	"insert into fetch_all5_tab values (2, 'def')",
	"insert into fetch_all5_tab values (3, 'ghi')"
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

$s = oci_parse($c, "select * from fetch_all5_tab order by 1");
oci_execute($s);
$r = oci_fetch_all($s, $res, 0, -1);
var_dump($r);
var_dump($res);

echo "Test 1\n";

$s = oci_parse($c, "select * from fetch_all5_tab order by 1");
oci_execute($s);
$r = oci_fetch_all($s, $res, 0, 0);
var_dump($r);
var_dump($res);

echo "Test 3\n";

$s = oci_parse($c, "select * from fetch_all5_tab order by 1");
oci_execute($s);
$r = oci_fetch_all($s, $res, -1, 0);
var_dump($r);
var_dump($res);

// Clean up

$stmtarray = array(
	"drop table fetch_all5_tab"
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
int(3)
array(2) {
  [%u|b%"MYCOL1"]=>
  array(3) {
    [0]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "2"
    [2]=>
    %unicode|string%(1) "3"
  }
  [%u|b%"MYCOL2"]=>
  array(3) {
    [0]=>
    %unicode|string%(3) "abc"
    [1]=>
    %unicode|string%(3) "def"
    [2]=>
    %unicode|string%(3) "ghi"
  }
}
Test 1
int(3)
array(2) {
  [%u|b%"MYCOL1"]=>
  array(3) {
    [0]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "2"
    [2]=>
    %unicode|string%(1) "3"
  }
  [%u|b%"MYCOL2"]=>
  array(3) {
    [0]=>
    %unicode|string%(3) "abc"
    [1]=>
    %unicode|string%(3) "def"
    [2]=>
    %unicode|string%(3) "ghi"
  }
}
Test 3
int(0)
array(0) {
}
===DONE===
