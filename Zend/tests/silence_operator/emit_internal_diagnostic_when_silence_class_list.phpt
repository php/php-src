--TEST--
Diagnostics should still be emitted if a class list is passed to @, internal
--FILE--
<?php

function test() {
    $r = $a + 1;
    return $r;
}

$var = @<Exception>test();

var_dump($var);

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
int(1)
Done
