--TEST--
Can't use name of lexical variable for parameter
--FILE--
<?php

$a = 1;
$fn = function ($a) use ($a) {
    var_dump($a);
};
$fn(2);

?>
--EXPECTF--
Fatal error: Lexical variable $a cannot be used as a parameter name in %s on line %d
