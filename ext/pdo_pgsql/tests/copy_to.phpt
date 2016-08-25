--TEST--
PDO PgSQL pgsqlCopyToArray and pgsqlCopyToFile
--SKIPIF--
<?php # vim:se ft=php:
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->exec('CREATE TABLE test (a integer not null primary key, b text, c integer)');

$db->beginTransaction();

echo "Preparing test table for CopyTo tests\n";
$stmt = $db->prepare("INSERT INTO test (a, b, c) values (?, ?, ?)");

for($i=0;$i<3;$i++) {
	$firstParameter = $i;
	$secondParameter = "test insert {$i}";
	$thirdParameter = NULL;
	$stmt->bindValue(1, $firstParameter);
	$stmt->bindValue(2, $secondParameter);
	$stmt->bindValue(3, $thirdParameter);
	$stmt->execute();
}

$db->commit();

echo "Testing pgsqlCopyToArray() with default parameters\n";
var_dump($db->pgsqlCopyToArray('test'));
echo "Testing pgsqlCopyToArray() with different field separator and not null indicator\n";
var_dump($db->pgsqlCopyToArray('test',";","NULL"));
echo "Testing pgsqlCopyToArray() with only selected fields\n";
var_dump($db->pgsqlCopyToArray('test',";","NULL",'a,c'));

echo "Testing pgsqlCopyToArray() with error\n";
try {
	var_dump($db->pgsqlCopyToArray('test_error'));
} catch (Exception $e) {
	echo "Exception: {$e->getMessage()}\n";
}

echo "Testing pgsqlCopyToFile() with default parameters\n";

$filename="test_pgsqlCopyToFile.csv";
var_dump($db->pgsqlCopyToFile('test',$filename));
echo file_get_contents($filename);
echo "Testing pgsqlCopyToFile() with different field separator and not null indicator\n";
var_dump($db->pgsqlCopyToFile('test',$filename,";","NULL"));
echo file_get_contents($filename);
echo "Testing pgsqlCopyToFile() with only selected fields\n";
var_dump($db->pgsqlCopyToFile('test',$filename,";","NULL",'a,c'));
echo file_get_contents($filename);

echo "Testing pgsqlCopyToFile() with error\n";
try {
	var_dump($db->pgsqlCopyToFile('test_error',$filename));
} catch (Exception $e) {
	echo "Exception: {$e->getMessage()}\n";
}

echo "Testing pgsqlCopyToFile() to unwritable file\n";
try {
	var_dump($db->pgsqlCopyToFile('test', 'nonexistent/foo.csv'));
} catch (Exception $e) {
	echo "Exception: {$e->getMessage()}\n";
}

if(isset($filename)) {
	@unlink($filename);
}
?>
--EXPECTF--
Preparing test table for CopyTo tests
Testing pgsqlCopyToArray() with default parameters
array(3) {
  [0]=>
  string(19) "0	test insert 0	\N
"
  [1]=>
  string(19) "1	test insert 1	\N
"
  [2]=>
  string(19) "2	test insert 2	\N
"
}
Testing pgsqlCopyToArray() with different field separator and not null indicator
array(3) {
  [0]=>
  string(21) "0;test insert 0;NULL
"
  [1]=>
  string(21) "1;test insert 1;NULL
"
  [2]=>
  string(21) "2;test insert 2;NULL
"
}
Testing pgsqlCopyToArray() with only selected fields
array(3) {
  [0]=>
  string(7) "0;NULL
"
  [1]=>
  string(7) "1;NULL
"
  [2]=>
  string(7) "2;NULL
"
}
Testing pgsqlCopyToArray() with error
Exception: SQLSTATE[42P01]: Undefined table: 7 %s:  %s "test_error" %s
Testing pgsqlCopyToFile() with default parameters
bool(true)
0	test insert 0	\N
1	test insert 1	\N
2	test insert 2	\N
Testing pgsqlCopyToFile() with different field separator and not null indicator
bool(true)
0;test insert 0;NULL
1;test insert 1;NULL
2;test insert 2;NULL
Testing pgsqlCopyToFile() with only selected fields
bool(true)
0;NULL
1;NULL
2;NULL
Testing pgsqlCopyToFile() with error
Exception: SQLSTATE[42P01]: Undefined table: 7 %s:  %s "test_error" %s
Testing pgsqlCopyToFile() to unwritable file
Exception: SQLSTATE[HY000]: General error: 7 Unable to open the file for writing

