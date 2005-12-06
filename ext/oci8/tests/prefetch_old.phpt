--TEST--
ocisetprefetch()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema.$table_name." (id, value) VALUES (1,1)";

if (!($s = ociparse($c, $insert_sql))) {
	die("ociparse(insert) failed!\n");
}

for ($i = 0; $i<3; $i++) {
	if (!ociexecute($s)) {
		die("ociexecute(insert) failed!\n");
	}
}

if (!ocicommit($c)) {
	die("ocicommit() failed!\n");
}

$select_sql = "SELECT * FROM ".$schema.$table_name."";

if (!($s = ociparse($c, $select_sql))) {
	die("ociparse(select) failed!\n");
}

var_dump(ocisetprefetch($s, 10));

if (!ociexecute($s)) {
	die("ociexecute(select) failed!\n");
}

var_dump(ocifetch($s));

var_dump(ocirowcount($s));

require dirname(__FILE__).'/drop_table.inc';
	
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
int(1)
Done
