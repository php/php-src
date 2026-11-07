--TEST--
Constructors cannot be async
--FILE--
<?php
class B { public async function __construct() {} }
?>
--EXPECTF--
Fatal error: Method __construct() cannot be async in %s on line %d
