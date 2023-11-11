--TEST--
Testing several callbacks using PDO::FETCH_FUNC
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$db->exec('CREATE TABLE testing (id INTEGER , name VARCHAR)');
$db->exec('INSERT INTO testing VALUES(1, "php")');
$db->exec('INSERT INTO testing VALUES(2, "")');

$st = $db->query('SELECT * FROM testing');
$st->fetchAll(PDO::FETCH_FUNC, function($x, $y) use ($st) { var_dump($st); print "data: $x, $y\n"; });

$st = $db->query('SELECT name FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, 'strtoupper'));

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, 'nothing'));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, ''));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, NULL));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, 1));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, array('self', 'foo')));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

class foo {
    public function method($x) {
        return "--- $x ---";
    }
}
class bar extends foo {
    public function __construct($db) {
        $st = $db->query('SELECT * FROM testing');
        var_dump($st->fetchAll(PDO::FETCH_FUNC, array($this, 'parent::method')));
    }

    static public function test($x, $y) {
        return $x .'---'. $y;
    }

    private function test2($x, $y) {
        return $x;
    }

    public function test3($x, $y) {
        return $x .'==='. $y;
    }
}

new bar($db);

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'test')));

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'test2')));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'test3')));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    $st = $db->query('SELECT * FROM testing');
    var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'inexistent')));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(21) "SELECT * FROM testing"
}
data: 1, php
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(21) "SELECT * FROM testing"
}
data: 2, 
array(2) {
  [0]=>
  string(3) "PHP"
  [1]=>
  string(0) ""
}
function "nothing" not found or invalid function name
function "" not found or invalid function name
PDOStatement::fetchAll(): Argument #2 must be a callable, null given
no array or string given
cannot access "self" when no class scope is active

Deprecated: Callables of the form ["bar", "parent::method"] are deprecated in %s on line %d
array(2) {
  [0]=>
  string(9) "--- 1 ---"
  [1]=>
  string(9) "--- 2 ---"
}
array(2) {
  [0]=>
  string(7) "1---php"
  [1]=>
  string(4) "2---"
}
non-static method bar::test2() cannot be called statically
non-static method bar::test3() cannot be called statically
class bar does not have a method "inexistent"
