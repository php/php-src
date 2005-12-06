--TEST--
oci_num_*() family
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

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

$select_sql = "SELECT * FROM ".$schema.$table_name."";

if (!($s = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

$delete_sql = "DELETE FROM ".$schema.$table_name."";

if (!($s = oci_parse($c, $delete_sql))) {
    die("oci_parse(delete) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(delete) failed!\n");
}
oci_commit($c);

var_dump(oci_num_rows($s));

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
int(0)
int(0)
int(5)
int(5)
int(3)
Done
