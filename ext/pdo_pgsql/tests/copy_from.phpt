--TEST--
PDO PgSQL pgsqlCopyFromArray and pgsqlCopyFromFile
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->exec('CREATE TABLE test (a integer not null primary key, b text, c integer)');

echo "Preparing test file and array for CopyFrom tests\n";

$tableRows = array();
$tableRowsWithDifferentNullValues = array();

for($i=0;$i<3;$i++) {
	$firstParameter = $i;
	$secondParameter = "test insert {$i}";
	$tableRows[] = "{$firstParameter}\t{$secondParameter}\t\\N";
	$tableRowsWithDifferentNullValues[] = "{$firstParameter};{$secondParameter};NULL";
	$tableRowsWithDifferentNullValuesAndSelectedFields[] = "{$firstParameter};NULL";
}
$filename = 'test_pgsqlCopyFromFile.csv';
$filenameWithDifferentNullValues = 'test_pgsqlCopyFromFileWithDifferentNullValues.csv';
$filenameWithDifferentNullValuesAndSelectedFields = 'test_pgsqlCopyFromFileWithDifferentNullValuesAndSelectedFields.csv';

file_put_contents($filename, implode("\n",$tableRows));
file_put_contents($filenameWithDifferentNullValues, implode("\n",$tableRowsWithDifferentNullValues));
file_put_contents($filenameWithDifferentNullValuesAndSelectedFields, implode("\n",$tableRowsWithDifferentNullValuesAndSelectedFields));

echo "Testing pgsqlCopyFromArray() with default parameters\n";
$db->beginTransaction();
var_dump($db->pgsqlCopyFromArray('test',$tableRows));

$stmt = $db->query("select * from test");
foreach($stmt as $r) {
	var_dump($r);
}
$db->rollback();

echo "Testing pgsqlCopyFromArray() with different field separator and not null indicator\n";
$db->beginTransaction();
var_dump($db->pgsqlCopyFromArray('test',$tableRowsWithDifferentNullValues,";","NULL"));
$stmt = $db->query("select * from test");
foreach($stmt as $r) {
	var_dump($r);
}
$db->rollback();

echo "Testing pgsqlCopyFromArray() with only selected fields\n";
$db->beginTransaction();
var_dump($db->pgsqlCopyFromArray('test',$tableRowsWithDifferentNullValuesAndSelectedFields,";","NULL",'a,c'));
$stmt = $db->query("select * from test");
foreach($stmt as $r) {
	var_dump($r);
}
$db->rollback();

echo "Testing pgsqlCopyFromArray() with error\n";
$db->beginTransaction();
try {
	var_dump($db->pgsqlCopyFromArray('test_error',$tableRowsWithDifferentNullValuesAndSelectedFields,";","NULL",'a,c'));
} catch (Exception $e) {
	echo "Exception: {$e->getMessage()}\n";
}
$db->rollback();

echo "Testing pgsqlCopyFromFile() with default parameters\n";
$db->beginTransaction();
var_dump($db->pgsqlCopyFromFile('test',$filename));

$stmt = $db->query("select * from test");
foreach($stmt as $r) {
	var_dump($r);
}
$db->rollback();

echo "Testing pgsqlCopyFromFile() with different field separator and not null indicator\n";
$db->beginTransaction();
var_dump($db->pgsqlCopyFromFile('test',$filenameWithDifferentNullValues,";","NULL"));
$stmt = $db->query("select * from test");
foreach($stmt as $r) {
	var_dump($r);
}
$db->rollback();

echo "Testing pgsqlCopyFromFile() with only selected fields\n";
$db->beginTransaction();
var_dump($db->pgsqlCopyFromFile('test',$filenameWithDifferentNullValuesAndSelectedFields,";","NULL",'a,c'));
$stmt = $db->query("select * from test");
foreach($stmt as $r) {
	var_dump($r);
}
$db->rollback();

echo "Testing pgsqlCopyFromFile() with error\n";
$db->beginTransaction();
try {
	var_dump($db->pgsqlCopyFromFile('test_error',$filenameWithDifferentNullValuesAndSelectedFields,";","NULL",'a,c'));
} catch (Exception $e) {
	echo "Exception: {$e->getMessage()}\n";
}
$db->rollback();

echo "Testing pgsqlCopyFromFile() with non existing file\n";
$db->beginTransaction();
try {
	var_dump($db->pgsqlCopyFromFile('test',"nonexisting/foo.csv",";","NULL",'a,c'));
} catch (Exception $e) {
	echo "Exception: {$e->getMessage()}\n";
}
$db->rollback();

// Clean up
foreach (array($filename, $filenameWithDifferentNullValues, $filenameWithDifferentNullValuesAndSelectedFields) as $f) {
	@unlink($f);
}
?>
--EXPECTF--
Preparing test file and array for CopyFrom tests
Testing pgsqlCopyFromArray() with default parameters
bool(true)
array(6) {
  ["a"]=>
  int(0)
  [0]=>
  int(0)
  ["b"]=>
  string(13) "test insert 0"
  [1]=>
  string(13) "test insert 0"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(1)
  [0]=>
  int(1)
  ["b"]=>
  string(13) "test insert 1"
  [1]=>
  string(13) "test insert 1"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(2)
  [0]=>
  int(2)
  ["b"]=>
  string(13) "test insert 2"
  [1]=>
  string(13) "test insert 2"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
Testing pgsqlCopyFromArray() with different field separator and not null indicator
bool(true)
array(6) {
  ["a"]=>
  int(0)
  [0]=>
  int(0)
  ["b"]=>
  string(13) "test insert 0"
  [1]=>
  string(13) "test insert 0"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(1)
  [0]=>
  int(1)
  ["b"]=>
  string(13) "test insert 1"
  [1]=>
  string(13) "test insert 1"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(2)
  [0]=>
  int(2)
  ["b"]=>
  string(13) "test insert 2"
  [1]=>
  string(13) "test insert 2"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
Testing pgsqlCopyFromArray() with only selected fields
bool(true)
array(6) {
  ["a"]=>
  int(0)
  [0]=>
  int(0)
  ["b"]=>
  NULL
  [1]=>
  NULL
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(1)
  [0]=>
  int(1)
  ["b"]=>
  NULL
  [1]=>
  NULL
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(2)
  [0]=>
  int(2)
  ["b"]=>
  NULL
  [1]=>
  NULL
  ["c"]=>
  NULL
  [2]=>
  NULL
}
Testing pgsqlCopyFromArray() with error
Exception: SQLSTATE[42P01]: Undefined table: 7 %s:  %stest_error%s
Testing pgsqlCopyFromFile() with default parameters
bool(true)
array(6) {
  ["a"]=>
  int(0)
  [0]=>
  int(0)
  ["b"]=>
  string(13) "test insert 0"
  [1]=>
  string(13) "test insert 0"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(1)
  [0]=>
  int(1)
  ["b"]=>
  string(13) "test insert 1"
  [1]=>
  string(13) "test insert 1"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(2)
  [0]=>
  int(2)
  ["b"]=>
  string(13) "test insert 2"
  [1]=>
  string(13) "test insert 2"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
Testing pgsqlCopyFromFile() with different field separator and not null indicator
bool(true)
array(6) {
  ["a"]=>
  int(0)
  [0]=>
  int(0)
  ["b"]=>
  string(13) "test insert 0"
  [1]=>
  string(13) "test insert 0"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(1)
  [0]=>
  int(1)
  ["b"]=>
  string(13) "test insert 1"
  [1]=>
  string(13) "test insert 1"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(2)
  [0]=>
  int(2)
  ["b"]=>
  string(13) "test insert 2"
  [1]=>
  string(13) "test insert 2"
  ["c"]=>
  NULL
  [2]=>
  NULL
}
Testing pgsqlCopyFromFile() with only selected fields
bool(true)
array(6) {
  ["a"]=>
  int(0)
  [0]=>
  int(0)
  ["b"]=>
  NULL
  [1]=>
  NULL
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(1)
  [0]=>
  int(1)
  ["b"]=>
  NULL
  [1]=>
  NULL
  ["c"]=>
  NULL
  [2]=>
  NULL
}
array(6) {
  ["a"]=>
  int(2)
  [0]=>
  int(2)
  ["b"]=>
  NULL
  [1]=>
  NULL
  ["c"]=>
  NULL
  [2]=>
  NULL
}
Testing pgsqlCopyFromFile() with error
Exception: SQLSTATE[42P01]: Undefined table: 7 %s:  %stest_error%s
Testing pgsqlCopyFromFile() with non existing file
Exception: SQLSTATE[HY000]: General error: 7 Unable to open the file
