--TEST--
GH-13597 (Allow disabling double-quoted string literals in SQLite)
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (!defined('Pdo\Sqlite::ATTR_DQS')) die('skip SQLite < 3.29.0');
?>
--FILE--
<?php

function check(PDO $pdo, string $sql): void {
    try {
        var_dump($pdo->query($sql)->fetchColumn());
    } catch (PDOException $e) {
        echo $e->getMessage(), "\n";
    }
}

$pdo = new Pdo\Sqlite('sqlite::memory:');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

echo "Enabled:\n";
var_dump($pdo->setAttribute(Pdo\Sqlite::ATTR_DQS, true));
var_dump($pdo->getAttribute(Pdo\Sqlite::ATTR_DQS));
check($pdo, 'SELECT "abc"');
$pdo->exec('CREATE TABLE t1 (a TEXT)');
$pdo->exec('CREATE INDEX t1_idx ON t1("missing")');
echo "index created\n";

echo "Disabled:\n";
var_dump($pdo->setAttribute(Pdo\Sqlite::ATTR_DQS, false));
var_dump($pdo->getAttribute(Pdo\Sqlite::ATTR_DQS));
check($pdo, 'SELECT "abc"');
check($pdo, "SELECT 'abc'");
check($pdo, 'SELECT "a" FROM t1 UNION ALL SELECT 1');
try {
    $pdo->exec('CREATE INDEX t1_idx2 ON t1("missing")');
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}

echo "Prepared statement:\n";
$pdo->setAttribute(Pdo\Sqlite::ATTR_DQS, true);
$stmt = $pdo->prepare('SELECT "abc"');
$stmt->execute();
var_dump($stmt->fetchColumn());
$pdo->setAttribute(Pdo\Sqlite::ATTR_DQS, false);
try {
    $stmt->execute();
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}
$pdo->setAttribute(Pdo\Sqlite::ATTR_DQS, true);
$stmt->execute();
var_dump($stmt->fetchColumn());

echo "Constructor option:\n";
$pdo = new Pdo\Sqlite('sqlite::memory:', options: [
    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
    Pdo\Sqlite::ATTR_DQS => false,
]);
var_dump($pdo->getAttribute(Pdo\Sqlite::ATTR_DQS));
check($pdo, 'SELECT "abc"');

?>
--EXPECTF--
Enabled:
bool(true)
bool(true)
string(3) "abc"
index created
Disabled:
bool(true)
bool(false)
SQLSTATE[HY000]: General error: 1 no such column: %Sabc%S
string(3) "abc"
int(1)
SQLSTATE[HY000]: General error: 1 no such column: %Smissing%S
Prepared statement:
string(3) "abc"
SQLSTATE[HY000]: General error: 1 no such column: %Sabc%S
string(3) "abc"
Constructor option:
bool(false)
SQLSTATE[HY000]: General error: 1 no such column: %Sabc%S
