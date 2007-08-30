--TEST--
oci_fetch_all()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__).'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema."".$table_name." (id, value) VALUES (1,1)";

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

$select_sql = "SELECT * FROM ".$schema."".$table_name."";

if (!($s = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

/* oci_fetch_all */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(oci_fetch_all($s, $all));
var_dump($all);

/* ocifetchstatement */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

var_dump(ocifetchstatement($s, $all));
var_dump($all);

require dirname(__FILE__).'/drop_table.inc';
	
echo "Done\n";
?>
--EXPECT--
int(3)
array(5) {
  ["ID"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  ["VALUE"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  ["BLOB"]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
  }
  ["CLOB"]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
  }
  ["STRING"]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
  }
}
int(3)
array(5) {
  ["ID"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  ["VALUE"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  ["BLOB"]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
  }
  ["CLOB"]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
  }
  ["STRING"]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    NULL
    [2]=>
    NULL
  }
}
Done
