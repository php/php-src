--TEST--
fetching cursor from a statement
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__."/create_table.inc";

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

$sql = "select CURSOR(select * from ".$schema.$table_name.") as curs FROM dual";
$stmt = oci_parse($c, $sql);

oci_execute($stmt);

while ($data = oci_fetch_assoc($stmt)) {
    oci_execute($data["CURS"]);
    $subdata = oci_fetch_assoc($data["CURS"]);
    var_dump($subdata);
    var_dump(oci_cancel($data["CURS"]));
    $subdata = oci_fetch_assoc($data["CURS"]);
    var_dump($subdata);
    var_dump(oci_cancel($data["CURS"]));
}

require __DIR__."/drop_table.inc";

echo "Done\n";

?>
--EXPECTF--
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
bool(true)

Warning: oci_fetch_assoc(): ORA-01002: fetch out of sequence in %s on line %d
bool(false)
bool(true)
Done
