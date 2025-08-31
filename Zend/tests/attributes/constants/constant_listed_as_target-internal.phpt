--TEST--
Constants listed in valid targets when used wrong (internal attribute)
--FILE--
<?php

#[Deprecated]
class Example {}

?>
--EXPECTF--
Fatal error: Attribute "Deprecated" cannot target class (allowed targets: function, method, class constant, constant) in %s on line %d
