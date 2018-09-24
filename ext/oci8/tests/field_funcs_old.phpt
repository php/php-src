--TEST--
ocicolumn*() family
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema."".$table_name." (id, value) VALUES (1,1)";

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

$select_sql = "SELECT * FROM ".$schema."".$table_name."";

if (!($s = ociparse($c, $select_sql))) {
	die("ociparse(select) failed!\n");
}

if (!ociexecute($s)) {
	die("ociexecute(select) failed!\n");
}

ocifetchinto($s, $row, OCI_NUM + OCI_RETURN_NULLS + OCI_RETURN_LOBS);
var_dump($row);

foreach ($row as $num => $field) {
	$num++;
	var_dump(ocicolumnisnull($s, $num));
	var_dump(ocicolumnname($s, $num));
	var_dump(ocicolumntype($s, $num));
	var_dump(ocicolumntyperaw($s, $num));
	var_dump(ocicolumnscale($s, $num));
	var_dump(ocicolumnprecision($s, $num));
	var_dump(ocicolumnsize($s, $num));
}


require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
array(5) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
}
bool(false)
string(2) "ID"
string(6) "NUMBER"
int(2)
int(-127)
int(0)
int(22)
bool(false)
string(5) "VALUE"
string(6) "NUMBER"
int(2)
int(-127)
int(0)
int(22)
bool(true)
string(4) "BLOB"
string(4) "BLOB"
int(113)
int(0)
int(0)
int(4000)
bool(true)
string(4) "CLOB"
string(4) "CLOB"
int(112)
int(0)
int(0)
int(4000)
bool(true)
string(6) "STRING"
string(8) "VARCHAR2"
int(1)
int(0)
int(0)
int(10)
Done
