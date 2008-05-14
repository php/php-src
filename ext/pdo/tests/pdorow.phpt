--TEST--
Trying instantiate a PDORow object manually
--FILE--
<?php

new PDORow;

?>
--EXPECTF--
Fatal error: PDORow::__construct(): You should not create a PDOStatement manually in %s on line %d
