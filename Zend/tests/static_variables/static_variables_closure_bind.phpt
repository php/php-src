--TEST--
Static variable can't override bound closure variables
--FILE--
<?php

$a = null;

function () use (&$a) {
    static $a;
};

?>
--EXPECTF--
Fatal error: Duplicate declaration of static variable $a in %s on line %d
