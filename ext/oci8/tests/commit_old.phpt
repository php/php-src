--TEST--
ocicommit()/ocirollback()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema.$table_name." (id, value) VALUES (1,1)";

if (!($s = ociparse($c, $insert_sql))) {
	die("ociparse(insert) failed!\n");
}

for ($i = 0; $i<3; $i++) {
	if (!ociexecute($s, OCI_DEFAULT)) {
		die("ociexecute(insert) failed!\n");
	}
}

var_dump(ocirollback($c));

$select_sql = "SELECT * FROM ".$schema.$table_name."";

if (!($select = ociparse($c, $select_sql))) {
	die("ociparse(select) failed!\n");
}

if (!oci_execute($select)) {
	die("ociexecute(select) failed!\n");
}
var_dump(ocifetchstatement($select, $all));
var_dump($all);

/* ocifetchstatement */
if (!ociexecute($s)) {
	die("ociexecute(select) failed!\n");
}

$insert_sql = "INSERT INTO ".$schema.$table_name." (id, value) VALUES (1,1)";

if (!($s = ociparse($c, $insert_sql))) {
    die("ociparse(insert) failed!\n");
}

for ($i = 0; $i<3; $i++) {
    if (!ociexecute($s, OCI_DEFAULT)) {
        die("ociexecute(insert) failed!\n");
    }
}

var_dump(ocicommit($c));

if (!ociexecute($select)) {
	die("ociexecute(select) failed!\n");
}
var_dump(ocifetchstatement($select, $all));
var_dump($all);


require dirname(__FILE__).'/drop_table.inc';
	
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
