--TEST--
Bug #35336 (crash on PDO::FETCH_CLASS + __set())
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
class EEE {
	function __set ($field, $value) {
		echo "hello world\n";
	}
}

$a = new PDO("sqlite::memory:");// pool ("sqlite::memory:");
$a->query ("CREATE TABLE test (a integer primary key, b text)");
$b = $a->prepare("insert into test (b) values (?)");
$b->execute(array (5));
$rez = $a->query ("SELECT * FROM test")->fetchAll(PDO::FETCH_CLASS, 'EEE');

echo "Done\n";
?>
--EXPECT--
hello world
hello world
Done
