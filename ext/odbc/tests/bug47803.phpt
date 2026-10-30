--TEST--
Bug #47803 Executing prepared statements is successful only for the first two statements
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include __DIR__ . "/config.inc";

$create_table = "CREATE TABLE bug47803(
        [PAR_ID] [int] NOT NULL,
        [PAR_INT] [int] NULL,
        [PAR_CHR] [varchar](500) NULL
)";

$inserts = "INSERT INTO bug47803
           ([PAR_ID]
           ,[PAR_INT]
           ,[PAR_CHR])
     VALUES
        (1,14,''),
        (2,30,''),
        (3,7,''),
        (4,7,''),
        (5,0,''),
        (6,0,''),
        (7,20130901,''),
        (8,20140201,''),
        (9,20140201,''),
        (10,20140620,''),
        (11,221,'')";


date_default_timezone_set('Europe/Warsaw');

$link = odbc_connect($dsn, $user, $pass);

odbc_exec($link, $create_table);
odbc_exec($link, $inserts);

$upd_params = array(
    array('id'=>1, 'name'=>'test 1'),
    array('id'=>2, 'name'=>'test 2'),
    array('id'=>3, 'name'=>'test 3'),
    array('id'=>4, 'name'=>'test 4'),
    array('id'=>5, 'name'=>'test 5'),
    array('id'=>10, 'name'=>'test 10'),
    array('id'=>9, 'name'=>'test 9'),
    array('id'=>8, 'name'=>'test 8'),
    array('id'=>7, 'name'=>'test 7'),
    array('id'=>6, 'name'=>'test 6'),
);
$sql = "UPDATE bug47803
     SET [PAR_CHR] = ?
     WHERE [PAR_ID] = ?";
$result = odbc_prepare($link, $sql);
if (!$result) {
    print ('[sql] prep: '.$sql);
    goto out;
}
foreach ($upd_params as &$k) {
    if(!odbc_execute($result, array($k['name'], $k['id']))) {
        print ('[sql] exec: '."array({$k['name']}, {$k['id']})");
        goto out;
    }
}
odbc_free_result($result);

$sql = "SELECT * FROM bug47803 WHERE [PAR_ID] = ?";
$result = odbc_prepare($link, $sql);
if (!$result) {
    print ('[sql] prep: '.$sql);
    goto out;
}
foreach ($upd_params as $k) {
    if(!odbc_execute($result, array($k['id']))) {
        print ('[sql] exec: '."array({$k['id']})");
        goto out;
    }
    while (($r = odbc_fetch_array($result)) !== false) {
        var_dump($r);
    }
}

out:
if ($result) odbc_free_result($result);
odbc_close($link);

?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE bug47803');

odbc_close($conn);

?>
--EXPECT--
array(3) {
  ["PAR_ID"]=>
  string(1) "1"
  ["PAR_INT"]=>
  string(2) "14"
  ["PAR_CHR"]=>
  string(6) "test 1"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "2"
  ["PAR_INT"]=>
  string(2) "30"
  ["PAR_CHR"]=>
  string(6) "test 2"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "3"
  ["PAR_INT"]=>
  string(1) "7"
  ["PAR_CHR"]=>
  string(6) "test 3"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "4"
  ["PAR_INT"]=>
  string(1) "7"
  ["PAR_CHR"]=>
  string(6) "test 4"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "5"
  ["PAR_INT"]=>
  string(1) "0"
  ["PAR_CHR"]=>
  string(6) "test 5"
}
array(3) {
  ["PAR_ID"]=>
  string(2) "10"
  ["PAR_INT"]=>
  string(8) "20140620"
  ["PAR_CHR"]=>
  string(7) "test 10"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "9"
  ["PAR_INT"]=>
  string(8) "20140201"
  ["PAR_CHR"]=>
  string(6) "test 9"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "8"
  ["PAR_INT"]=>
  string(8) "20140201"
  ["PAR_CHR"]=>
  string(6) "test 8"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "7"
  ["PAR_INT"]=>
  string(8) "20130901"
  ["PAR_CHR"]=>
  string(6) "test 7"
}
array(3) {
  ["PAR_ID"]=>
  string(1) "7"
  ["PAR_INT"]=>
  string(8) "20130901"
  ["PAR_CHR"]=>
  string(6) "test 7"
}
