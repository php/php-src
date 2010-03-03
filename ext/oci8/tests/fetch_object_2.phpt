--TEST--
oci_fetch_object() with CLOB and NULL
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table fetch_object_2_tab",
	"create table fetch_object_2_tab (col1 number, col2 CLOB, col3 varchar2(15))",
	"insert into fetch_object_2_tab values (123, '1st row col2 string', '1 more text')",
	"insert into fetch_object_2_tab values (456, '2nd row col2 string', NULL)",
	"insert into fetch_object_2_tab values (789, '3rd row col2 string', '3 more text')",
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

if (!($s = oci_parse($c, 'select * from fetch_object_2_tab order by 1'))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
	var_dump($row);
}

echo "Test 2\n";

if (!($s = oci_parse($c, 'select * from fetch_object_2_tab order by 1'))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
    echo $row->COL1 . "\n";
    echo $row->COL2->load(100) . "\n";
    echo $row->COL3 . "\n";
}

// Clean up

$stmtarray = array(
	"drop table fetch_object_2_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
object(stdClass)#%d (3) {
  [%u|b%"COL1"]=>
  %unicode|string%(3) "123"
  [%u|b%"COL2"]=>
  object(OCI-Lob)#%d (1) {
    [%u|b%"descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  [%u|b%"COL3"]=>
  %unicode|string%(11) "1 more text"
}
object(stdClass)#%d (3) {
  [%u|b%"COL1"]=>
  %unicode|string%(3) "456"
  [%u|b%"COL2"]=>
  object(OCI-Lob)#%d (1) {
    [%u|b%"descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  [%u|b%"COL3"]=>
  NULL
}
object(stdClass)#%d (3) {
  [%u|b%"COL1"]=>
  %unicode|string%(3) "789"
  [%u|b%"COL2"]=>
  object(OCI-Lob)#%d (1) {
    [%u|b%"descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  [%u|b%"COL3"]=>
  %unicode|string%(11) "3 more text"
}
Test 2
123
1st row col2 string
1 more text
456
2nd row col2 string

789
3rd row col2 string
3 more text
===DONE===
