--TEST--
Bug #40415 (Using oci_fetchall with nested cursors)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";

// Setup

$create_1 = "CREATE TABLE t1 (id1 INTEGER)";
$create_2 = "CREATE TABLE t2 (id2 INTEGER)";
$drop_1 = "DROP TABLE t1";
$drop_2 = "DROP TABLE t2";

$s1 = oci_parse($c, $drop_1);
$s2 = oci_parse($c, $drop_2);
@oci_execute($s1);
@oci_execute($s2);

$s1 = oci_parse($c, $create_1);
$s2 = oci_parse($c, $create_2);
oci_execute($s1);
oci_execute($s2);

for($i=1; $i < 4; $i++) {
    $insert = "INSERT INTO t1 VALUES(1".$i.")";
    $s = oci_parse($c, $insert);
    oci_execute($s);
}

for($i=1; $i < 4; $i++) {
    $insert = "INSERT INTO t2 VALUES(2".$i.")";
    $s = oci_parse($c, $insert);
    oci_execute($s);
}


function do_assoc($c)
{
    $query = "SELECT t1.*, CURSOR( SELECT * FROM t2 ) AS CURSOR FROM t1";

    $stmt = oci_parse($c, $query);
    oci_execute($stmt);

    while ($row = oci_fetch_assoc($stmt)) {
        print "Got row \"".$row['ID1']."\". Now getting nested cursor:\n";
        var_dump(oci_execute($row['CURSOR']));
        while ($row_n = oci_fetch_assoc($row['CURSOR']) ) {
            var_dump($row_n);
        }
    }
}

function do_all($c)
{
    $query = "SELECT t1.*, CURSOR( SELECT * FROM t2 ) AS CURSOR FROM t1";

    $stmt = oci_parse($c, $query);
    oci_execute($stmt);

    $rc1 = oci_fetch_all($stmt, $res);

    echo "Rows returned $rc1\n";

    var_dump($res);

    foreach ($res['CURSOR'] as $cv) {
        echo "Getting nested cursor\n";
        var_dump(oci_execute($cv));
        $rc2 = oci_fetch_all($cv, $res2);
        var_dump($res2);
    }
}



echo "Test 1: Associate fetch of nested cursor\n";
do_assoc($c);

echo "\nTest 2: fetchall of nested cursor\n";
do_all($c);


// Cleanup
$s1 = oci_parse($c, $drop_1);
$s2 = oci_parse($c, $drop_2);
@oci_execute($s1);
@oci_execute($s2);

echo "Done\n";
?>
--EXPECTF--
Test 1: Associate fetch of nested cursor
Got row "11". Now getting nested cursor:
bool(true)
array(1) {
  ["ID2"]=>
  string(2) "21"
}
array(1) {
  ["ID2"]=>
  string(2) "22"
}
array(1) {
  ["ID2"]=>
  string(2) "23"
}
Got row "12". Now getting nested cursor:
bool(true)
array(1) {
  ["ID2"]=>
  string(2) "21"
}
array(1) {
  ["ID2"]=>
  string(2) "22"
}
array(1) {
  ["ID2"]=>
  string(2) "23"
}
Got row "13". Now getting nested cursor:
bool(true)
array(1) {
  ["ID2"]=>
  string(2) "21"
}
array(1) {
  ["ID2"]=>
  string(2) "22"
}
array(1) {
  ["ID2"]=>
  string(2) "23"
}

Test 2: fetchall of nested cursor
Rows returned 3
array(2) {
  ["ID1"]=>
  array(3) {
    [0]=>
    string(2) "11"
    [1]=>
    string(2) "12"
    [2]=>
    string(2) "13"
  }
  ["CURSOR"]=>
  array(3) {
    [0]=>
    resource(%d) of type (oci8 statement)
    [1]=>
    resource(%d) of type (oci8 statement)
    [2]=>
    resource(%d) of type (oci8 statement)
  }
}
Getting nested cursor
bool(true)
array(1) {
  ["ID2"]=>
  array(3) {
    [0]=>
    string(2) "21"
    [1]=>
    string(2) "22"
    [2]=>
    string(2) "23"
  }
}
Getting nested cursor
bool(true)
array(1) {
  ["ID2"]=>
  array(3) {
    [0]=>
    string(2) "21"
    [1]=>
    string(2) "22"
    [2]=>
    string(2) "23"
  }
}
Getting nested cursor
bool(true)
array(1) {
  ["ID2"]=>
  array(3) {
    [0]=>
    string(2) "21"
    [1]=>
    string(2) "22"
    [2]=>
    string(2) "23"
  }
}
Done
