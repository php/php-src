--TEST--
GC support for PDO Sqlite driver data
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class Obj {
	public $a;
	public function callback() { }
}

$obj = new Obj;
$obj->a = new PDO('sqlite::memory:');
$obj->a->sqliteCreateFunction('func1', function() use ($obj) {}, 1);
$obj->a->sqliteCreateAggregate('func2', function() use ($obj) {}, function() use($obj) {});
$obj->a->sqliteCreateCollation('col', function() use ($obj) {});

?>
===DONE===
--EXPECT--
===DONE===
