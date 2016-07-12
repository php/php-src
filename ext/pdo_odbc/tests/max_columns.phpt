--TEST--
PDO ODBC varying character with max/no length
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_odbc')) print 'skip not loaded';
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_odbc/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

if (false === $db->exec('CREATE TABLE TEST (id INT NOT NULL PRIMARY KEY, data varchar(max))')) {
	if (false === $db->exec('CREATE TABLE TEST (id INT NOT NULL PRIMARY KEY, data longtext)')) {
		if (false === $db->exec('CREATE TABLE TEST (id INT NOT NULL PRIMARY KEY, data CLOB)')) {
			die("BORK: don't know how to create a long column here:\n" . implode(", ", $db->errorInfo()));
		}
	}
}

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$sizes = array(32, 64, 128, 253, 254, 255, 256, 257, 258, 512, 1024, 2048, 3998, 3999, 4000);

$db->beginTransaction();
$insert = $db->prepare('INSERT INTO TEST VALUES (?, ?)');
foreach ($sizes as $num) {
	$insert->execute(array($num, str_repeat('i', $num)));
}
$insert = null;
$db->commit();

foreach ($db->query('SELECT id, data from TEST') as $row) {
	$expect = str_repeat('i', $row[0]);
	if (strcmp($expect, $row[1])) {
		echo "Failed on size $row[id]:\n";
		printf("Expected %d bytes, got %d\n", strlen($expect), strlen($row['data']));
		echo bin2hex($expect) . "\n";
		echo bin2hex($row['data']) . "\n";
	}
}

echo "Finished\n";

--EXPECT--
Finished
