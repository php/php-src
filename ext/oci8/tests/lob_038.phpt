--TEST--
Array fetch CLOB and BLOB
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';
require __DIR__.'/create_table.inc';

echo "Test 1: CLOB\n";

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (clob)
                      VALUES (empty_clob())
                      RETURNING
                               clob
                      INTO :v_clob ";

$s = oci_parse($c,$ora_sql);
$clob = oci_new_descriptor($c,OCI_DTYPE_LOB);


oci_bind_by_name($s,":v_clob", $clob,-1,OCI_B_CLOB);

oci_execute($s, OCI_DEFAULT);
var_dump($clob->save("clob test 1"));

oci_execute($s, OCI_DEFAULT);
var_dump($clob->save("clob test 2"));

oci_execute($s, OCI_DEFAULT);
var_dump($clob->save("clob test 3"));


$s = oci_parse($c,"select clob from ".$schema.$table_name);
var_dump(oci_execute($s));

oci_fetch_all($s, $res);

var_dump($res);


echo "Test 1b\n";

$s = oci_parse($c, "select clob from ".$schema.$table_name);
var_dump(oci_execute($s, OCI_DEFAULT));
while ($row = oci_fetch_array($s, OCI_ASSOC)) {
    var_dump($row);
    $result = $row['CLOB']->load();
    var_dump($result);
}


require __DIR__.'/drop_table.inc';

echo "Test 2: BLOB\n";

require __DIR__.'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (blob)
                      VALUES (empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$s = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_DTYPE_LOB);


oci_bind_by_name($s,":v_blob", $blob,-1,OCI_B_BLOB);

oci_execute($s, OCI_DEFAULT);
var_dump($blob->save("blob test 1"));

oci_execute($s, OCI_DEFAULT);
var_dump($blob->save("blob test 2"));

oci_execute($s, OCI_DEFAULT);
var_dump($blob->save("blob test 3"));

$s = oci_parse($c, "select blob from ".$schema.$table_name);
var_dump(oci_execute($s));
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 2b\n";

$s = oci_parse($c, "select blob from ".$schema.$table_name);
var_dump(oci_execute($s, OCI_DEFAULT));
while ($row = oci_fetch_array($s, OCI_ASSOC)) {
    var_dump($row);
    $result = $row['BLOB']->load();
    var_dump($result);
}


require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
Test 1: CLOB
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  ["CLOB"]=>
  array(3) {
    [0]=>
    string(11) "clob test 1"
    [1]=>
    string(11) "clob test 2"
    [2]=>
    string(11) "clob test 3"
  }
}
Test 1b
bool(true)
array(1) {
  ["CLOB"]=>
  object(OCILob)#2 (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(11) "clob test 1"
array(1) {
  ["CLOB"]=>
  object(OCILob)#3 (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(11) "clob test 2"
array(1) {
  ["CLOB"]=>
  object(OCILob)#2 (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(11) "clob test 3"
Test 2: BLOB
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  ["BLOB"]=>
  array(3) {
    [0]=>
    string(11) "blob test 1"
    [1]=>
    string(11) "blob test 2"
    [2]=>
    string(11) "blob test 3"
  }
}
Test 2b
bool(true)
array(1) {
  ["BLOB"]=>
  object(OCILob)#3 (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(11) "blob test 1"
array(1) {
  ["BLOB"]=>
  object(OCILob)#4 (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(11) "blob test 2"
array(1) {
  ["BLOB"]=>
  object(OCILob)#3 (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(11) "blob test 3"
Done
