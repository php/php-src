--TEST--
fetching cursor from a statement
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_table.inc";

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

$sql = "select CURSOR(select * from ".$schema.$table_name.") as curs FROM dual";
$stmt = ociparse($c, $sql);

ociexecute($stmt);

while ($result = ocifetchinto($stmt, $data, OCI_ASSOC)) {
	ociexecute($data["CURS"]);
	ocifetchinto($data["CURS"], $subdata, OCI_ASSOC);
	var_dump($subdata);
	var_dump(ocicancel($data["CURS"]));
	ocifetchinto($data["CURS"], $subdata, OCI_ASSOC);
	var_dump($subdata);
	var_dump(ocicancel($data["CURS"]));
}

require dirname(__FILE__)."/drop_table.inc";

echo "Done\n";

?>
--EXPECTF--
array(2) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
bool(true)

Warning: ocifetchinto():%sORA-01002: fetch out of sequence in %scursors_old.php on line %d
array(2) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
}
bool(true)
Done
