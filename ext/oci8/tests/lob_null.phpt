--TEST--
Test null data for CLOBs
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

// Initialization

$s = oci_parse($c, 'drop table lob_null_tab');
@oci_execute($s);

$s = oci_parse($c, 'create table lob_null_tab (id number, data clob)');
oci_execute($s);

$s = oci_parse($c,
'create or replace procedure lob_null_proc_in (pid in number, pdata in CLOB)
 as begin
   insert into lob_null_tab (id, data) values (pid, pdata);
 end;');
oci_execute($s);

$s = oci_parse($c,
'create or replace procedure lob_null_proc_out (pid in number, pdata out clob)
   as begin
     select data into pdata from lob_null_tab where id = pid;
 end;');
oci_execute($s);

// TEMPORARY CLOB

echo "Temporary CLOB: NULL\n";
$s = oci_parse($c, "insert into lob_null_tab values (1, :b)");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary(null);
$r = @oci_execute($s);
if (!$r) {
    $m = oci_error($s);
    echo $m['message'], "\n";
}
else {
    $lob->close();
}

echo "Temporary CLOB: ''\n";
$s = oci_parse($c, "insert into lob_null_tab values (2, :b)");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary('');
$r = @oci_execute($s);
if (!$r) {
    $m = oci_error($s);
    echo $m['message'], "\n";
}
else {
    $lob->close();
}

echo "Temporary CLOB: text\n";
$s = oci_parse($c, "insert into lob_null_tab values (3, :b)");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary('Inserted via SQL statement');
$r = @oci_execute($s);
if (!$r) {
    $m = oci_error($s);
    echo $m['message'], "\n";
}
else {
    $lob->close();
}

// PROCEDURE PARAMETER

echo "Procedure parameter: NULL\n";
$s = oci_parse($c, "call lob_null_proc_in(4, :b)");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary(null);
$r = @oci_execute($s);
if (!$r) {
    $m = oci_error($s);
    echo $m['message'], "\n";
}
else {
    $lob->close();
}

echo "Procedure parameter: ''\n";
$s = oci_parse($c, "call lob_null_proc_in(5, :b)");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary('');
$r = @oci_execute($s);
if (!$r) {
    $m = oci_error($s);
    echo $m['message'], "\n";
}
else {
    $lob->close();
}

echo "Procedure parameter: text\n";
$s = oci_parse($c, "call lob_null_proc_in(6, :b)");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary('Inserted via procedure parameter');
$r = @oci_execute($s);
if (!$r) {
    $m = oci_error($s);
    echo $m['message'], "\n";
}
else {
    $lob->close();
}

// RETURNING INTO

echo "RETURNING INTO: null\n";
$s = oci_parse($c, "insert into lob_null_tab values (7, empty_clob()) returning data into :b");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);  // Must have OCI_DEFAULT here so locator is still valid
$lob->save(null);

echo "RETURNING INTO: ''\n";
$s = oci_parse($c, "insert into lob_null_tab values (8, empty_clob()) returning data into :b");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);  // Must have OCI_DEFAULT here so locator is still valid
$lob->save('');

echo "RETURNING INTO: text\n";
$s = oci_parse($c, "insert into lob_null_tab values (9, empty_clob()) returning data into :b");
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);  // Must have OCI_DEFAULT here so locator is still valid
$lob->save('Inserted with RETURNING INTO');

echo "Fetch as string\n";
$s = oci_parse ($c, 'select id, data from lob_null_tab order by id');
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "\nFetch as a descriptor\n";
$s = oci_parse ($c, 'select id, data from lob_null_tab order by id');
oci_execute($s);
while ($arr = oci_fetch_assoc($s)) {
    if (is_object($arr['DATA'])) {
        echo $arr['ID'] . " is an object: ";
        $r = $arr['DATA']->load();
        var_dump($r);
    }
    else {
        echo $arr['ID'] . " is not an object\n";
    }
}

echo "\nFetch via the procedure parameter\n";
for ($i = 1; $i <= 9; $i++)
{
    $s = oci_parse ($c, "call lob_null_proc_out($i, :b)");
    $lob = oci_new_descriptor($c, OCI_D_LOB);
    oci_bind_by_name($s, ':b', $lob, -1, OCI_B_CLOB);
    oci_execute($s);
    if (is_object($lob)) {
        echo $i . " is an object: ";
        $r = $lob->load();
        var_dump($r);
    }
    else {
        echo $i . " is not an object\n";
    }
}

// Cleanup

$s = oci_parse($c, 'drop table lob_null_tab');
@oci_execute($s);

echo "Done\n";

?>
--EXPECT--
Temporary CLOB: NULL
Temporary CLOB: ''
Temporary CLOB: text
Procedure parameter: NULL
Procedure parameter: ''
Procedure parameter: text
RETURNING INTO: null
RETURNING INTO: ''
RETURNING INTO: text
Fetch as string
array(2) {
  ["ID"]=>
  array(9) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
    [3]=>
    string(1) "4"
    [4]=>
    string(1) "5"
    [5]=>
    string(1) "6"
    [6]=>
    string(1) "7"
    [7]=>
    string(1) "8"
    [8]=>
    string(1) "9"
  }
  ["DATA"]=>
  array(9) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
    [2]=>
    string(26) "Inserted via SQL statement"
    [3]=>
    string(0) ""
    [4]=>
    string(0) ""
    [5]=>
    string(32) "Inserted via procedure parameter"
    [6]=>
    string(0) ""
    [7]=>
    string(0) ""
    [8]=>
    string(28) "Inserted with RETURNING INTO"
  }
}

Fetch as a descriptor
1 is an object: string(0) ""
2 is an object: string(0) ""
3 is an object: string(26) "Inserted via SQL statement"
4 is an object: string(0) ""
5 is an object: string(0) ""
6 is an object: string(32) "Inserted via procedure parameter"
7 is an object: string(0) ""
8 is an object: string(0) ""
9 is an object: string(28) "Inserted with RETURNING INTO"

Fetch via the procedure parameter
1 is an object: string(0) ""
2 is an object: string(0) ""
3 is an object: string(26) "Inserted via SQL statement"
4 is an object: string(0) ""
5 is an object: string(0) ""
6 is an object: string(32) "Inserted via procedure parameter"
7 is an object: string(0) ""
8 is an object: string(0) ""
9 is an object: string(28) "Inserted with RETURNING INTO"
Done
