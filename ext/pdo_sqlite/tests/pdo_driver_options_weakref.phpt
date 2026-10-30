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
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$value = null;
$driverOptions = [new Tracked()];
$weakReference = WeakReference::create($driverOptions[0]);
var_dump(@$stmt->bindColumn('missing', $value, PDO::PARAM_STR, 0, $driverOptions));
unset($driverOptions);
var_dump($weakReference->get());
unset($value, $stmt);
?>
--EXPECT--
bool(true)
NULL
bool(false)
NULL
