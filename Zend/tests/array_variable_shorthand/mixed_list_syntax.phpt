--TEST--
Array variable shorthand does not change the list() and [] mixing restriction
--FILE--
<?php
$record = ['user' => ['name' => 'Ada']];
list('user' => [:$name]) = $record;
?>
--EXPECTF--
Fatal error: Cannot mix [] and list() in %s on line %d
