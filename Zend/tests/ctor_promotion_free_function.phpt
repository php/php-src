--TEST--
Constructor promotion cannot be used in a free function
--FILE--
<?php

function __construct(public $prop) {}

?>
--EXPECTF--
Fatal error: __construct(): Promoted property $prop must be declared inside a constructor in %s on line %d
