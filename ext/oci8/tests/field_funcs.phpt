--TEST--
oci_field_*() family
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__.'/create_table.inc';

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

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

$row = oci_fetch_array($s, OCI_NUM + OCI_RETURN_NULLS + OCI_RETURN_LOBS);
var_dump($row);

foreach ($row as $num => $field) {
    $num++;
    var_dump(oci_field_is_null($s, $num));
    var_dump(oci_field_name($s, $num));
    var_dump(oci_field_type($s, $num));
    var_dump(oci_field_type_raw($s, $num));
    var_dump(oci_field_scale($s, $num));
    var_dump(oci_field_precision($s, $num));
    var_dump(oci_field_size($s, $num));
}


require __DIR__.'/drop_table.inc';

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
