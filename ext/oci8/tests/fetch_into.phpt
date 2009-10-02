--TEST--
ocifetchinto()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_into_tab",
    "create table fetch_into_tab (id number, value number)",
    "insert into fetch_into_tab (id, value) values (1,1)",
    "insert into fetch_into_tab (id, value) values (1,1)",
    "insert into fetch_into_tab (id, value) values (1,1)",
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
                        942 // table or view does not exist
                ))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

// Run Test

if (!($s = oci_parse($c, "select * from fetch_into_tab"))) {
	die("oci_parse(select) failed!\n");
}

/* ocifetchinto */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(ocifetchinto($s, $all));
var_dump($all);

/* ocifetchinto */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(ocifetchinto($s, $all, OCI_NUM+OCI_ASSOC+OCI_RETURN_NULLS+OCI_RETURN_LOBS));
var_dump($all);

// Cleanup

$stmtarray = array(
    "drop table fetch_into_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}
	
echo "Done\n";
?>
--EXPECTF--
int(2)
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
}
int(2)
array(4) {
  [0]=>
  %unicode|string%(1) "1"
  [%u|b%"ID"]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
  [%u|b%"VALUE"]=>
  %unicode|string%(1) "1"
}
Done
