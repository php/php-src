--TEST--
PDO OCI: Inserts 10K with 1 number and 2 LOB columns (stress test)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(dirname(__FILE__) . '/../../pdo/tests/pdo_test.inc');

$db = PDOTest::factory();

$query = "begin execute immediate 'drop table pdo_oci_stream_2'; exception when others then if sqlcode <> -942 then raise; end if; end;";
$stmt = $db->prepare($query);
$stmt->execute();

$query = "create table pdo_oci_stream_2 (id number, data1 blob, data2 blob)";
$stmt = $db->prepare($query);
$stmt->execute();

function do_insert($db, $id, $data1, $data2)
{
    $db->beginTransaction();
    $stmt = $db->prepare("insert into pdo_oci_stream_2 (id, data1, data2) values (:id, empty_blob(), empty_blob()) returning data1, data2 into :blob1, :blob2");
    $stmt->bindParam(':id', $id);
    $stmt->bindParam(':blob1', $blob1, PDO::PARAM_LOB);
    $stmt->bindParam(':blob2', $blob2, PDO::PARAM_LOB);
    $blob1 = null;
    $blob2 = null;
    $stmt->execute();

    fwrite($blob1, $data1);
    fclose($blob1);
    fwrite($blob2, $data2);
    fclose($blob2);
    $db->commit();
}

$a1 = str_repeat('a', 4086);
$a2 = str_repeat('b', 4087);
$a3 = str_repeat('c', 4088);
$a4 = str_repeat('d', 4089);
$a5 = str_repeat('e', 4090);
$a6 = str_repeat('f', 4091);
$a7 = str_repeat('g', 4092);
$a8 = str_repeat('h', 4093);
$a9 = str_repeat('i', 4094);
$a10 = str_repeat('j', 4095);

printf("Inserting 10000 Records ... ");
for($i=0; $i<1000; $i++) {
    do_insert($db, $i * 10 + 1, $a1, $a10);
    do_insert($db, $i * 10 + 2, $a2, $a9);
    do_insert($db, $i * 10 + 3, $a3, $a8);
    do_insert($db, $i * 10 + 4, $a4, $a7);
    do_insert($db, $i * 10 + 5, $a5, $a6);
    do_insert($db, $i * 10 + 6, $a6, $a5);
    do_insert($db, $i * 10 + 7, $a7, $a4);
    do_insert($db, $i * 10 + 8, $a8, $a3);
    do_insert($db, $i * 10 + 9, $a9, $a2);
    do_insert($db, $i * 10 + 10, $a10, $a1);
}
printf("Done\n");

/* Cleanup is done in pdo_oci_stream_2b.phpt */
//$db->exec("drop table pdo_oci_stream_2");

?>
--EXPECT--
Inserting 10000 Records ... Done
