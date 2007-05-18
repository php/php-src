--TEST--
bind and fetch cursor from a statement
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$drop_table = "DROP TABLE ".$schema.$table_name."";

if (!($s = oci_parse($c, $drop_table))) {
	die("oci_parse(drop) failed!\n");
}

@oci_execute($s);

$create_table = "CREATE TABLE ".$schema.$table_name." (id NUMBER, value VARCHAR(20))";

if (!($s = oci_parse($c, $create_table))) {
	die("oci_parse(create) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(create) failed!\n");
}

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


$sql = "
DECLARE
TYPE curtype IS REF CURSOR;
cursor_var curtype;
BEGIN
	OPEN cursor_var FOR SELECT id, value FROM ".$schema.$table_name.";
	:curs := cursor_var;
END;
";

$stmt = oci_parse($c, $sql);

$cursor = oci_new_cursor($c);
oci_bind_by_name($stmt, ":curs", $cursor, -1, OCI_B_CURSOR);

oci_execute($stmt);

oci_execute($cursor);
var_dump(oci_fetch_row($cursor));
var_dump(oci_fetch_row($cursor));
var_dump(oci_fetch_row($cursor));
var_dump(oci_fetch_row($cursor));

echo "Done\n";

$drop_table = "DROP TABLE ".$schema.$table_name."";

if (!($s = oci_parse($c, $drop_table))) {
	die("oci_parse(drop) failed!\n");
}

@oci_execute($s);

?>
--EXPECT--	
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
bool(false)
Done
