--TEST--
Constructor promotion cannot be used in a free function
--FILE--
<?php

function __construct(public $prop) {}

?>
--EXPECTF--
Fatal error: Cannot declare promoted property outside a constructor in %s on line %d
