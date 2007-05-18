--TEST--
oci_define_by_name()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_table.inc";

$insert_sql = "INSERT INTO ".$schema.$table_name." (string) VALUES ('some')";

if (!($s = oci_parse($c, $insert_sql))) {
        die("oci_parse(insert) failed!\n");
}

if (!oci_execute($s)) {
        die("oci_execute(insert) failed!\n");
}

$stmt = oci_parse($c, "SELECT string FROM ".$table_name."");

/* the define MUST be done BEFORE ociexecute! */

$strong = '';
oci_define_by_name($stmt, "STRING", $string, 20);

oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($string);
}

require dirname(__FILE__)."/drop_table.inc";

echo "Done\n";

?>
--EXPECT--
string(4) "some"
Done
