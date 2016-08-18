--TEST--
PDO OCI Bug #57702 (Multi-row BLOB fetches)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require('ext/pdo/tests/pdo_test.inc');
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');

// Note the PDO test setup sets PDO::ATTR_STRINGIFY_FETCHES to true
// (and sets PDO::ATTR_CASE to PDO::CASE_LOWER)

$query = "begin execute immediate 'drop table bug57702'; exception when others then if sqlcode <> -942 then raise; end if; end;";
$stmt = $db->prepare($query);
$stmt->execute();

$query = "create table bug57702 (id number, data1 blob, data2 blob)";
$stmt = $db->prepare($query);
$stmt->execute();

function do_insert($db, $id, $data1, $data2)
{
	$db->beginTransaction(); 
	$stmt = $db->prepare("insert into bug57702 (id, data1, data2) values (:id, empty_blob(), empty_blob()) returning data1, data2 into :blob1, :blob2");
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

do_insert($db, 1, "row 1 col 1", "row 1 col 2");
do_insert($db, 2, "row 2 col 1", "row 2 col 2");

////////////////////

echo "First Query\n";

// Fetch it back
$stmt = $db->prepare('select data1, data2 from bug57702 order by id');
$stmt->execute();
$row = $stmt->fetch(PDO::FETCH_ASSOC);
var_dump($row['data1']);
var_dump($row['data2']);
$row = $stmt->fetch(PDO::FETCH_ASSOC);
var_dump($row['data1']);
var_dump($row['data2']);

////////////////////

echo "\nSecond Query\n";

foreach($db->query("select data1 as d1, data2 as d2 from bug57702 order by id") as $row) {
	var_dump($row['d1']);
	var_dump($row['d2']);
}

////////////////////

echo "\nThird Query\n";

$stmt = $db->prepare('select data1 as d3_1, data2 as d3_2 from bug57702 order by id');

$rs = $stmt->execute();
$stmt->bindColumn('d3_1' , $clob1, PDO::PARAM_LOB);
$stmt->bindColumn('d3_2' , $clob2, PDO::PARAM_LOB);

while ($stmt->fetch(PDO::FETCH_BOUND)) {
    var_dump($clob1);
    var_dump($clob2);
}

////////////////////

echo "\nFourth Query\n"; 

$a = array();
$i = 0;
foreach($db->query("select data1 as d4_1, data2 as d4_2 from bug57702 order by id") as $row) {
	$a[$i][0] = $row['d4_1'];
	$a[$i][1] = $row['d4_2'];
    $i++;
}

for ($i = 0; $i < count($a); $i++) {
    var_dump($a[$i][0]);
    var_dump($a[$i][1]);
}

////////////////////

echo "\nFifth Query\n"; 

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);  // Let's use streams

// Since each column only has one lob descriptor, the last row is
// shown twice because the lob descriptor for each column is reused in
// the stream

$a = array();
$i = 0;
foreach($db->query("select data1 as d4_1, data2 as d4_2 from bug57702 order by id") as $row) {
	$a[$i][0] = $row['d4_1'];
	$a[$i][1] = $row['d4_2'];
    $i++;
}

for ($i = 0; $i < count($a); $i++) {
    var_dump(stream_get_contents($a[$i][0]));
    var_dump(stream_get_contents($a[$i][1]));
}

////////////////////

echo "\nSixth Query\n"; 

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);  // Let's use streams

$a = array();
$i = 0;
foreach($db->query("select data1 as d4_1, data2 as d4_2 from bug57702 order by id") as $row) {
	$a[$i][0] = $row['d4_1'];
	$a[$i][1] = $row['d4_2'];
	var_dump(stream_get_contents($a[$i][0]));
	var_dump(stream_get_contents($a[$i][1]));
	$i++;
}

// Cleanup
$query = "drop table bug57702";
$stmt = $db->prepare($query);
$stmt->execute();

print "done\n";

?>
--EXPECTF--
First Query
string(11) "row 1 col 1"
string(11) "row 1 col 2"
string(11) "row 2 col 1"
string(11) "row 2 col 2"

Second Query
string(11) "row 1 col 1"
string(11) "row 1 col 2"
string(11) "row 2 col 1"
string(11) "row 2 col 2"

Third Query
string(11) "row 1 col 1"
string(11) "row 1 col 2"
string(11) "row 2 col 1"
string(11) "row 2 col 2"

Fourth Query
string(11) "row 1 col 1"
string(11) "row 1 col 2"
string(11) "row 2 col 1"
string(11) "row 2 col 2"

Fifth Query
string(11) "row 2 col 1"
string(11) "row 2 col 2"
string(11) "row 2 col 1"
string(11) "row 2 col 2"

Sixth Query
string(11) "row 1 col 1"
string(11) "row 1 col 2"
string(11) "row 2 col 1"
string(11) "row 2 col 2"
done
