--TEST--
Test ROWID bind
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

function do_query($c)
{
    $s = oci_parse($c, 'select address from rid_tab order by id');
    $id = 1;
    oci_execute($s, OCI_DEFAULT);
    while ($row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS)) {
        var_dump($row);
    }
}

$stmtarray = array(
    "drop table rid_tab",
    "create table rid_tab (id number, address varchar2(40))",
    "insert into rid_tab (id, address) values (1, 'original text #1')",
    "insert into rid_tab (id, address) values (2, 'original text #2')"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Initial Data\n";
do_query($c);

$s = oci_parse($c, 'select rowid, address from rid_tab where id = :l_bv for update');
$id = 1;
oci_bind_by_name($s, ':l_bv', $id);
oci_execute($s, OCI_DEFAULT);
$row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS);

$rid = $row['ROWID'];
$addr = $row['ADDRESS'];

$addr = 'Some new text';

// Save changes
$s = oci_parse($c,'update rid_tab set address = :a_bv where rowid = :r_bv');
oci_bind_by_name($s, ':r_bv', $rid, -1, OCI_B_ROWID);
oci_bind_by_name($s, ':a_bv', $addr);
oci_execute($s);

echo "Verify Change\n";
do_query($c);

// Cleanup

$stmtarray = array(
    "drop table rid_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Initial Data
array(1) {
  ["ADDRESS"]=>
  string(16) "original text #1"
}
array(1) {
  ["ADDRESS"]=>
  string(16) "original text #2"
}
Verify Change
array(1) {
  ["ADDRESS"]=>
  string(13) "Some new text"
}
array(1) {
  ["ADDRESS"]=>
  string(16) "original text #2"
}
Done
