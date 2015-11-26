--TEST--
Testing several callbacks using PDO::FETCH_FUNC
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE testing (id INTEGER , name VARCHAR)');
$db->exec('INSERT INTO testing VALUES(1, "php")');
$db->exec('INSERT INTO testing VALUES(2, "")');

$st = $db->query('SELECT * FROM testing');
$st->fetchAll(PDO::FETCH_FUNC, function($x, $y) use ($st) { var_dump($st); print "data: $x, $y\n"; });

$st = $db->query('SELECT name FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, 'strtoupper'));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, 'nothing'));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, ''));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, NULL));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, 1));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, array('self', 'foo')));

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

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'test2')));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'test3')));

$st = $db->query('SELECT * FROM testing');
var_dump($st->fetchAll(PDO::FETCH_FUNC, array('bar', 'inexistent')));

?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  [%u|b%"queryString"]=>
  %string|unicode%(21) "SELECT * FROM testing"
}
data: 1, php
object(PDOStatement)#%d (1) {
  [%u|b%"queryString"]=>
  %string|unicode%(21) "SELECT * FROM testing"
}
data: 2, 
array(2) {
  [0]=>
  %string|unicode%(3) "PHP"
  [1]=>
  %string|unicode%(0) ""
}

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: function 'nothing' not found or invalid function name in %s on line %d
bool(false)

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: function '' not found or invalid function name in %s on line %d
bool(false)

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: no array or string given in %s on line %d
bool(false)

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: no array or string given in %s on line %d
bool(false)

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: class 'PDOStatement' does not have a method 'foo' in %s on line %d
bool(false)
array(2) {
  [0]=>
  %string|unicode%(9) "--- 1 ---"
  [1]=>
  %string|unicode%(9) "--- 2 ---"
}
array(2) {
  [0]=>
  %string|unicode%(7) "1---php"
  [1]=>
  %string|unicode%(4) "2---"
}

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: cannot access private method bar::test2() in %s on line %d
bool(false)
array(2) {
  [0]=>
  %string|unicode%(7) "1===php"
  [1]=>
  %string|unicode%(4) "2==="
}

Warning: PDOStatement::fetchAll(): SQLSTATE[HY000]: General error: class 'bar' does not have a method 'inexistent' in %s on line %d
bool(false)
