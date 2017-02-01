--TEST--
Calling parse_str through argument unpacking
--FILE--
<?php

function test() {
    $i = 0;
    parse_str(...["i=41"]);
    var_dump($i + 1);
}
test();

?>
--EXPECTF--
Deprecated: parse_str(): Calling parse_str() without the result argument is deprecated in %s on line %d
int(42)
