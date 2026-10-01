--TEST--
PDO PgSQL Pdo\Pgsql::ATTR_CHUNK_SIZE rejects values it cannot honour
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
if (!defined('Pdo\Pgsql::ATTR_CHUNK_SIZE')) die('skip libpq >= 17 required');
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "=== a negative value is refused by setAttribute() ===\n";
try {
    $pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, -1);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo "the attribute kept its previous value: ";
var_dump($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));

echo "=== and by the prepare() driver options ===\n";
try {
    $pdo->prepare("SELECT 1", [Pdo\Pgsql::ATTR_CHUNK_SIZE => -42]);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "=== and by the constructor options ===\n";
$user = getenv('PDOTEST_USER');
$pass = getenv('PDOTEST_PASS');
try {
    new PDO(getenv('PDOTEST_DSN'), $user === false ? null : $user,
        $pass === false ? null : $pass, [Pdo\Pgsql::ATTR_CHUNK_SIZE => -1]);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "=== values that are not integers ===\n";
foreach ([1.5, "8", [8], null, new stdClass()] as $value) {
    try {
        $pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, $value);
        printf("%-8s accepted as %d\n", get_debug_type($value),
            $pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
    } catch (TypeError $e) {
        printf("%-8s %s: %s\n", get_debug_type($value), $e::class, $e->getMessage());
    }
}
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 0);

echo "=== scrollable cursors have nothing to stream ===\n";
try {
    $pdo->prepare("SELECT 1", [
        PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL,
        Pdo\Pgsql::ATTR_CHUNK_SIZE => 4,
    ]);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 4);
$stmt = $pdo->prepare("SELECT 1", [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
$stmt->execute();
echo "an inherited chunk size leaves the cursor alone: ";
var_dump($stmt->fetchColumn());
unset($stmt);
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 0);

echo "=== a rejected value leaves the connection usable ===\n";
var_dump($pdo->query("SELECT 1")->fetchColumn());

?>
--EXPECT--
=== a negative value is refused by setAttribute() ===
ValueError: Pdo\Pgsql::ATTR_CHUNK_SIZE must be between 0 and 2147483647
the attribute kept its previous value: int(0)
=== and by the prepare() driver options ===
ValueError: Pdo\Pgsql::ATTR_CHUNK_SIZE must be between 0 and 2147483647
=== and by the constructor options ===
ValueError: Pdo\Pgsql::ATTR_CHUNK_SIZE must be between 0 and 2147483647
=== values that are not integers ===
float    TypeError: Attribute value must be of type int for selected attribute, float given
string   accepted as 8
array    TypeError: Attribute value must be of type int for selected attribute, array given
null     TypeError: Attribute value must be of type int for selected attribute, null given
stdClass TypeError: Attribute value must be of type int for selected attribute, stdClass given
=== scrollable cursors have nothing to stream ===
ValueError: Pdo\Pgsql::ATTR_CHUNK_SIZE cannot be combined with PDO::ATTR_CURSOR set to PDO::CURSOR_SCROLL
an inherited chunk size leaves the cursor alone: string(1) "1"
=== a rejected value leaves the connection usable ===
string(1) "1"
