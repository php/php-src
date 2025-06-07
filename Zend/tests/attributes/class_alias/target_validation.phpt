--TEST--
Can only be used on classes
--FILE--
<?php

#[ClassAlias]
function demo() {}

?>
--EXPECTF--
Fatal error: Attribute "ClassAlias" cannot target function (allowed targets: class) in %s on line %d
