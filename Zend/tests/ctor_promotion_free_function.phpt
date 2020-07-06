--TEST--
Constructor promotion cannot be used in a free function
--FILE--
<?php

function __construct(public $prop) {}

?>
--EXPECTF--
Fatal error: __construct(): Promoted property $prop cannot be declared outside a constructor in %s on line %d
