--TEST--
Alias name must be valid
--FILE--
<?php

#[ClassAlias('never')]
class Demo {}

?>
--EXPECTF--
Fatal error: Cannot use "never" as a class alias as it is reserved in %s on line %d
