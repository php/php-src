--TEST--
PDO_DBLIB: Column data types, with or without stringifying
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$sql = "
	SELECT
		'foo' AS [char],
		CAST('2030-01-01 23:59:59' AS DATETIME) AS [datetime],
		CAST(0 AS BIT) AS [false],
		10.5 AS [float],
		1000 AS [int],
		CAST(10.5 AS MONEY) AS [money],
		CAST('1950-01-18 23:00:00' AS SMALLDATETIME) as [smalldatetime],
		CAST(1 AS BIT) AS [true]
";

$stmt = $db->query($sql);
$row = $stmt->fetch(PDO::FETCH_ASSOC);

var_dump($row['char'] === 'foo');
var_dump($row['datetime'] === '2030-01-01 23:59:59');
var_dump($row['false'] === 0);
var_dump($row['float'] === 10.5);
var_dump($row['int'] === 1000);
var_dump($row['money'] === 10.5);
var_dump($row['smalldatetime'] === '1950-01-18 23:00:00');
var_dump($row['true'] === 1);

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
$stmt = $db->query($sql);
$row = $stmt->fetch(PDO::FETCH_ASSOC);

var_dump($row['char'] === 'foo');
var_dump($row['datetime'] === '2030-01-01 23:59:59');
var_dump($row['false'] === '0');
var_dump($row['float'] === '10.5');
var_dump($row['int'] === '1000');
var_dump($row['money'] === '10.5');
var_dump($row['smalldatetime'] === '1950-01-18 23:00:00');
var_dump($row['true'] === '1');

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
