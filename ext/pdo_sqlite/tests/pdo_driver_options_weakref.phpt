--TEST--
PDO SQLite releases driverOptions objects after binding or failed registration
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
class Tracked {}

$db = new PDO('sqlite::memory:');

$stmt = $db->prepare('SELECT ? AS value');
$value = 1;
$driverOptions = [new Tracked()];
$weakReference = WeakReference::create($driverOptions[0]);
var_dump($stmt->bindParam(1, $value, PDO::PARAM_STR, 0, $driverOptions));
unset($driverOptions, $value, $stmt);
gc_collect_cycles();
var_dump($weakReference->get());

$stmt = $db->prepare('SELECT ? AS value');
$stmt->execute();
$value = null;
$driverOptions = [new Tracked()];
$weakReference = WeakReference::create($driverOptions[0]);
try {
    $stmt->bindColumn('missing', $value, PDO::PARAM_STR, 0, $driverOptions);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
unset($e, $driverOptions);
var_dump($weakReference->get());
unset($value, $stmt);
?>
--EXPECT--
bool(true)
NULL
ValueError: PDOStatement::bindColumn(): Argument #1 ($column) must refer to a column present in the result set, "missing" given
NULL
