--TEST--
oci_set_prefetch()
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

var_dump(oci_set_prefetch($s, 10));

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

var_dump(oci_fetch($s));

var_dump(oci_num_rows($s));

require dirname(__FILE__).'/drop_table.inc';
	
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
int(1)
Done
