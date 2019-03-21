--TEST--
oci_fetch_all() - 2
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__.'/create_table.inc';

$insert_sql = "INSERT INTO ".$schema."".$table_name." (id, value) VALUES (1,1)";

$s = oci_parse($c, $insert_sql);

for ($i = 0; $i<3; $i++) {
	oci_execute($s);
}

oci_commit($c);

$select_sql = "SELECT * FROM ".$schema."".$table_name."";

$s = oci_parse($c, $select_sql);

oci_execute($s);
var_dump(oci_fetch_all($s, $all));
var_dump($all);

oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, 10, OCI_FETCHSTATEMENT_BY_ROW));
var_dump($all);

oci_execute($s);
var_dump(oci_fetch_all($s, $all, -1, -1, OCI_FETCHSTATEMENT_BY_ROW));
var_dump($all);

oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, 2, OCI_FETCHSTATEMENT_BY_ROW+OCI_NUM));
var_dump($all);

oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, 2, OCI_NUM));
var_dump($all);

oci_execute($s);
var_dump(oci_fetch_all($s, $all, 0, 1, OCI_BOTH));
var_dump($all);

require __DIR__.'/drop_table.inc';

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
array(3) {
  [0]=>
  array(5) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(1) "1"
    ["BLOB"]=>
    NULL
    ["CLOB"]=>
    NULL
    ["STRING"]=>
    NULL
  }
  [1]=>
  array(5) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(1) "1"
    ["BLOB"]=>
    NULL
    ["CLOB"]=>
    NULL
    ["STRING"]=>
    NULL
  }
  [2]=>
  array(5) {
    ["ID"]=>
    string(1) "1"
    ["VALUE"]=>
    string(1) "1"
    ["BLOB"]=>
    NULL
    ["CLOB"]=>
    NULL
    ["STRING"]=>
    NULL
  }
}
int(0)
array(0) {
}
int(2)
array(2) {
  [0]=>
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
  [1]=>
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
}
int(2)
array(5) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
  [3]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
  [4]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
}
int(1)
array(5) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
  [2]=>
  array(1) {
    [0]=>
    NULL
  }
  [3]=>
  array(1) {
    [0]=>
    NULL
  }
  [4]=>
  array(1) {
    [0]=>
    NULL
  }
}
Done
