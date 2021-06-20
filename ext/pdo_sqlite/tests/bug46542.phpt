--TEST--
Bug #46542 Extending PDO class with a __call() function
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
class A extends PDO
{ function __call($m, $p) {print __CLASS__."::$m\n";} }

$a = new A('sqlite:' . __DIR__ . '/dummy.db');

$a->truc();
$a->TRUC();

?>
--CLEAN--
<?php
unlink(__DIR__ . '/dummy.db');
?>
--EXPECT--
A::truc
A::TRUC
