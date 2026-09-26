--TEST--
PDO SQLite: failed execute invalidates the previous result
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');

$stmt = $db->prepare('SELECT ? AS value UNION ALL SELECT 2');
$stmt->execute(['first']);
var_dump($stmt->fetchColumn());

try {
    $stmt->execute([new stdClass()]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($stmt->fetchColumn());

$stmt->execute(['third']);
var_dump($stmt->fetchAll(PDO::FETCH_COLUMN));

$db->sqliteCreateFunction('fail_execute', function (bool $fail): string {
    if ($fail) {
        throw new RuntimeException('execution failed');
    }
    return 'first';
});
$stmt = $db->prepare('SELECT fail_execute(?) AS value UNION ALL SELECT 2');
$stmt->execute([false]);
var_dump($stmt->fetchColumn());

try {
    $stmt->execute([true]);
} catch (RuntimeException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($stmt->fetchColumn());

$stmt->execute([false]);
var_dump($stmt->fetchAll(PDO::FETCH_COLUMN));

$db->exec('CREATE TABLE failed_execute (value INTEGER UNIQUE)');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
$stmt = $db->prepare('INSERT INTO failed_execute VALUES (?)');
$stmt->execute([1]);
$warnings = 0;
set_error_handler(function () use (&$warnings): bool {
    $warnings++;
    return true;
});
try {
    var_dump($stmt->execute([1]));
} finally {
    restore_error_handler();
}
echo "driver warnings: $warnings\n";
var_dump($stmt->fetchColumn());

?>
--EXPECT--
string(5) "first"
Error: Object of class stdClass could not be converted to string
bool(false)
array(2) {
  [0]=>
  string(5) "third"
  [1]=>
  int(2)
}
string(5) "first"
RuntimeException: execution failed
bool(false)
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  int(2)
}
bool(false)
driver warnings: 1
bool(false)
