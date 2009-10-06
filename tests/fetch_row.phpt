--TEST--
oci_fetch_row()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_row_tab",
    "create table fetch_row_tab (id number, value number)",
    "insert into fetch_row_tab (id, value) values (1,1)",
    "insert into fetch_row_tab (id, value) values (1,1)",
    "insert into fetch_row_tab (id, value) values (1,1)",
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

if (!($s = oci_parse($c, "select * from fetch_row_tab"))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_row($s)) {
	var_dump($row);
}

// Cleanup

$stmtarray = array(
    "drop table fetch_row_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

echo "Done\n";

?>
--EXPECTF--
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
}
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
}
array(2) {
  [0]=>
  %unicode|string%(1) "1"
  [1]=>
  %unicode|string%(1) "1"
}
Done
