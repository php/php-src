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

function get_expected_float_string() {
	switch (get_tds_version()) {
		case '5.0':
		case '6.0':
		case '7.0':
		case '7.1':
		case '7.2':
		case '8.0':
			return '10.500';
		default:
			return '10.5';
	}
}

$sql = "
	SELECT
		'foo' AS [char],
		CAST('2030-01-01 23:59:59' AS DATETIME) AS [datetime],
		CAST(0 AS BIT) AS [false],
		10.500 AS [float],
		1000 AS [int],
		CAST(10.500 AS MONEY) AS [money],
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
var_dump($row['float'] === get_expected_float_string());
var_dump($row['int'] === '1000');
var_dump($row['money'] === '10.50');
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
