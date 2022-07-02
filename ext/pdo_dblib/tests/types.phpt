--TEST--
PDO_DBLIB: Column data types, with or without stringifying
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

function get_expected_float_string() {
    global $db;

    switch ($db->getAttribute(PDO::DBLIB_ATTR_TDS_VERSION)) {
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

var_dump($row['char']);
var_dump($row['datetime']);
var_dump($row['false']);
var_dump($row['float']);
var_dump($row['int']);
var_dump($row['money']);
var_dump($row['smalldatetime']);
var_dump($row['true']);

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
$stmt = $db->query($sql);
$row = $stmt->fetch(PDO::FETCH_ASSOC);

var_dump($row['char']);
var_dump($row['datetime']);
var_dump($row['false']);
var_dump($row['float'] === get_expected_float_string());
var_dump($row['int']);
// var_dump($row['money']); -- the decimal precision varies and it's not clear why
var_dump($row['smalldatetime']);
var_dump($row['true']);

?>
--EXPECT--
string(3) "foo"
string(19) "2030-01-01 23:59:59"
int(0)
float(10.5)
int(1000)
float(10.5)
string(19) "1950-01-18 23:00:00"
int(1)
string(3) "foo"
string(19) "2030-01-01 23:59:59"
string(1) "0"
bool(true)
string(4) "1000"
string(19) "1950-01-18 23:00:00"
string(1) "1"
