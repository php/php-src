--TEST--
Test OCI_NO_AUTO_COMMIT constant
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__."/connect.inc");
require(__DIR__.'/create_table.inc');

$insert_sql = "insert into ".$schema.$table_name." (id, value) values (1,1)";

if (!($s = oci_parse($c, $insert_sql))) {
	die("oci_parse(insert) failed!\n");
}

/* check with OCI_NO_AUTO_COMMIT mode  */
for ($i = 0; $i<3; $i++) {
	if (!oci_execute($s, OCI_NO_AUTO_COMMIT)) {
		die("oci_execute(insert) failed!\n");
	}
}

for ($i = 0; $i<3; $i++) {
	if (!oci_execute($s, OCI_DEFAULT)) {
		die("oci_execute(insert) failed!\n");
	}
}


var_dump(oci_rollback($c));

$select_sql = "select * from ".$schema.$table_name."";

if (!($select = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

/* oci_fetch_all */
if (!oci_execute($select)) {
	die("oci_execute(select) failed!\n");
}
var_dump(oci_fetch_all($select, $all));
var_dump($all);

/* ocifetchstatement */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

$insert_sql = "insert into ".$schema.$table_name." (id, value) values (1,1)";

if (!($s = oci_parse($c, $insert_sql))) {
    die("oci_parse(insert) failed!\n");
}

for ($i = 0; $i<3; $i++) {
    if (!oci_execute($s, OCI_DEFAULT)) {
        die("oci_execute(insert) failed!\n");
    }
}

var_dump(oci_commit($c));

/* oci_fetch_all */
if (!oci_execute($select)) {
	die("oci_execute(select) failed!\n");
}
var_dump(oci_fetch_all($select, $all));
var_dump($all);


require(__DIR__.'/drop_table.inc');

echo "Done\n";
?>
--EXPECT--
bool(true)
int(0)
array(5) {
  ["ID"]=>
  array(0) {
  }
  ["VALUE"]=>
  array(0) {
  }
  ["BLOB"]=>
  array(0) {
  }
  ["CLOB"]=>
  array(0) {
  }
  ["STRING"]=>
  array(0) {
  }
}
bool(true)
int(4)
array(5) {
  ["ID"]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
    [3]=>
    string(1) "1"
  }
  ["VALUE"]=>
  array(4) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
    [3]=>
    string(1) "1"
  }
  ["BLOB"]=>
  array(4) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
    [3]=>
    NULL
  }
  ["CLOB"]=>
  array(4) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
    [3]=>
    NULL
  }
  ["STRING"]=>
  array(4) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
    [3]=>
    NULL
  }
}
Done
