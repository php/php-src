--TEST--
GH-24233 (PDO_PGSQL silently truncates bound strings containing NUL bytes)
--CREDITS--
Raj Siva-Rajah
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require_once __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require_once __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require_once __DIR__ . '/config.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$value = "hello\0world";

foreach ([false, true] as $emulate) {
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, $emulate);
    echo 'emulate prepares: ', var_export($emulate, true), PHP_EOL;

    $stmt = $db->prepare('SELECT CAST(? AS text)');
    try {
        $stmt->execute([$value]);
        var_dump($stmt->fetchColumn());
    } catch (PDOException $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }

    $stmt = $db->prepare('SELECT CAST(? AS text), CAST(? AS text)');
    try {
        $stmt->execute(['ok', $value]);
        var_dump($stmt->fetch(PDO::FETCH_NUM));
    } catch (PDOException $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }

    $stmt = $db->prepare('SELECT length(CAST(? AS bytea))');
    $stmt->bindValue(1, $value, PDO::PARAM_LOB);
    $stmt->execute();
    var_dump((int) $stmt->fetchColumn());
}

try {
    var_dump($db->quote($value));
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($db->quote($value, PDO::PARAM_LOB));

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
var_dump($db->quote($value));

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
var_dump($db->quote($value));

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$stmt = $db->prepare('SELECT CAST(? AS text)');
var_dump($stmt->execute([$value]));
var_dump($stmt->errorInfo());
?>
--EXPECTF--
emulate prepares: false
PDOException: SQLSTATE[HY000]: General error: parameter 1 must not contain any null bytes
PDOException: SQLSTATE[HY000]: General error: parameter 2 must not contain any null bytes
int(11)
emulate prepares: true
PDOException: Pgsql PDO::quote does not support null bytes
PDOException: Pgsql PDO::quote does not support null bytes
int(11)
PDOException: Pgsql PDO::quote does not support null bytes
string(26) "'\x68656c6c6f00776f726c64'"

Warning: PDO::quote(): Pgsql PDO::quote does not support null bytes in %s on line %d
bool(false)
bool(false)
bool(false)
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  NULL
  [2]=>
  string(43) "parameter 1 must not contain any null bytes"
}
