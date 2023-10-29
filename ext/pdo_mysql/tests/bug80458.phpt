--TEST--
Bug #80458 PDOStatement::fetchAll() throws for upsert queries
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$db->query('DROP TABLE IF EXISTS test');
$db->query('CREATE TABLE test (first int) ENGINE = InnoDB');
$res = $db->query('INSERT INTO test(first) VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9),(10),(11),(12),(13),(14),(15),(16)');
var_dump($res->fetchAll());

$stmt = $db->prepare('DELETE FROM test WHERE first=1');
$stmt->execute();
var_dump($stmt->fetchAll());

$res = $db->query('DELETE FROM test WHERE first=2');
var_dump($res->fetchAll());

$stmt2 = $db->prepare('DELETE FROM test WHERE first=3');
$stmt2->execute();
foreach($stmt2 as $row){
    // expect nothing
}

$stmt3 = $db->prepare('DELETE FROM test WHERE first=4');
$stmt3->execute();
var_dump($stmt3->fetch(PDO::FETCH_ASSOC));

$stmt = $db->prepare('SELECT first FROM test WHERE first=5');
$stmt->execute();
var_dump($stmt->fetchAll());

$db->exec('DROP PROCEDURE IF EXISTS nores');
$db->exec('CREATE PROCEDURE nores() BEGIN DELETE FROM test WHERE first=6; END;');
$stmt4 = $db->prepare('CALL nores()');
$stmt4->execute();
var_dump($stmt4->fetchAll());
$db->exec('DROP PROCEDURE IF EXISTS nores');

$db->exec('DROP PROCEDURE IF EXISTS ret');
$db->exec('CREATE PROCEDURE ret() BEGIN SELECT first FROM test WHERE first=7; END;');
$stmt5 = $db->prepare('CALL ret()');
$stmt5->execute();
var_dump($stmt5->fetchAll());
$stmt5->nextRowset(); // needed to fetch the empty result set of CALL
var_dump($stmt5->fetchAll());
$db->exec('DROP PROCEDURE IF EXISTS ret');

/* With emulated prepares */
print("Emulated prepares\n");
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$stmt = $db->prepare('DELETE FROM test WHERE first=8');
$stmt->execute();
var_dump($stmt->fetchAll());

$res = $db->query('DELETE FROM test WHERE first=9');
var_dump($res->fetchAll());

$stmt2 = $db->prepare('DELETE FROM test WHERE first=10');
$stmt2->execute();
foreach($stmt2 as $row){
    // expect nothing
}

$stmt3 = $db->prepare('DELETE FROM test WHERE first=11');
$stmt3->execute();
var_dump($stmt3->fetch(PDO::FETCH_ASSOC));

$stmt = $db->prepare('SELECT first FROM test WHERE first=12');
$stmt->execute();
var_dump($stmt->fetchAll());

$db->exec('DROP PROCEDURE IF EXISTS nores');
$db->exec('CREATE PROCEDURE nores() BEGIN DELETE FROM test WHERE first=13; END;');
$stmt4 = $db->prepare('CALL nores()');
$stmt4->execute();
var_dump($stmt4->fetchAll());
$db->exec('DROP PROCEDURE IF EXISTS nores');

$db->exec('DROP PROCEDURE IF EXISTS ret');
$db->exec('CREATE PROCEDURE ret() BEGIN SELECT first FROM test WHERE first=14; END;');
$stmt5 = $db->prepare('CALL ret()');
$stmt5->execute();
var_dump($stmt5->fetchAll());
$stmt5->nextRowset(); // needed to fetch the empty result set of CALL
var_dump($stmt5->fetchAll());
$db->exec('DROP PROCEDURE IF EXISTS ret');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);

$stmt = $db->prepare('DELETE FROM test WHERE first=15');
$stmt->execute();
var_dump($stmt->fetchAll());

$stmt = $db->prepare('SELECT first FROM test WHERE first=16');
$stmt->execute();
var_dump($stmt->fetchAll());

?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
bool(false)
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    string(1) "5"
    [0]=>
    string(1) "5"
  }
}
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    string(1) "7"
    [0]=>
    string(1) "7"
  }
}
array(0) {
}
Emulated prepares
array(0) {
}
array(0) {
}
bool(false)
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    string(2) "12"
    [0]=>
    string(2) "12"
  }
}
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    string(2) "14"
    [0]=>
    string(2) "14"
  }
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    string(2) "16"
    [0]=>
    string(2) "16"
  }
}
