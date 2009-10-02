--TEST--
binding a cursor (with errors)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table cursor_bind_err_tab",
    "create table cursor_bind_err_tab (id number, value number)",
    "insert into cursor_bind_err_tab (id, value) values (1,1)",
    "insert into cursor_bind_err_tab (id, value) values (1,1)",
    "insert into cursor_bind_err_tab (id, value) values (1,1)",
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

$sql = "select cursor(select * from cursor_bind_err_tab) into :cursor from dual";
$stmt = oci_parse($c, $sql);

$cursor = oci_new_cursor($c);
oci_bind_by_name($stmt, ":cursor", $cursor, -1, OCI_B_CURSOR);

oci_execute($stmt);

oci_execute($cursor);
var_dump(oci_fetch_assoc($cursor));

// Cleanup

$stmtarray = array(
    "drop table cursor_bind_err_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

echo "Done\n";

?>
--EXPECTF--
Warning: oci_bind_by_name(): ORA-01036: %s in %s on line %d

Warning: oci_fetch_assoc(): ORA-24338: %s in %s on line %d
bool(false)
Done
