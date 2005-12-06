--TEST--
binding a cursor (with errors)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_table.inc";

$insert_sql = "INSERT INTO ".$schema.$table_name." (id, value) VALUES (1,1)";

if (!($s = oci_parse($c, $insert_sql))) {
	die("oci_parse(insert) failed!\n");
}

for ($i = 0; $i<3; $i++) {
	if (!oci_execute($s)) {
		die("oci_execute(insert) failed!\n");
	}
}

if (!oci_commit($c)) {
	die("oci_commit() failed!\n");
}

$sql = "select CURSOR(select * from ".$schema.$table_name.") into :cursor from dual";
$stmt = oci_parse($c, $sql);

$cursor = oci_new_cursor($c);
oci_bind_by_name($stmt, ":cursor", $cursor, -1, OCI_B_CURSOR);

oci_execute($stmt);

oci_execute($cursor);
var_dump(oci_fetch_assoc($cursor));

require dirname(__FILE__)."/drop_table.inc";

echo "Done\n";

?>
--EXPECTF--
Warning: oci_bind_by_name(): ORA-01036: illegal variable name/number in %s on line %d

Warning: oci_fetch_assoc(): ORA-24338: statement handle not executed in %s on line %d
bool(false)
Done
