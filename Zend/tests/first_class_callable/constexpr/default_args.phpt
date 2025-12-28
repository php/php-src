--TEST--
FCC in default argument
--FILE--
<?php

function test(
    Closure $name = strrev(...)
) {
    var_dump($name("abc"));
}

test();
test(strlen(...));

?>
--EXPECT--
string(3) "cba"
int(3)
