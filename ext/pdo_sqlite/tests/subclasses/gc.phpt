--TEST--
GC support for PDO Sqlite driver data
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

// This test was copied from the pdo_sqlite test for sqliteCreateAggregate
class Obj {
	public $a;
	public function callback() { }
}

$obj = new Obj;
$obj->a = Pdo::connect('sqlite::memory:');

if (!$obj->a instanceof PdoSqlite) {
    echo "Wrong class type. Should be PdoSqlite but is " . get_class($obj->a) . "]\n";
}

$obj->a->createFunction('func1', function() use ($obj) {}, 1);
$obj->a->createAggregate('func2', function() use ($obj) {}, function() use($obj) {});
$obj->a->createCollation('col', function() use ($obj) {});

?>
===DONE===
--EXPECT--
===DONE===
