--TEST--
Bug #46873 (extract($foo) crashes if $foo['foo'] exists)
--FILE--
<?php
$foo = array('foo' => 1, 'bar' => 2, 'test' => 3);
extract($foo);
var_dump($foo, $bar, $test);
?>
--EXPECT--
int(1)
int(2)
int(3)
