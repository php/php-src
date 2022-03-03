--TEST--
Assign elision exception safety: UCALL
--EXTENSIONS--
opcache
--FILE--
<?php

function test() {
    $dtor = new class { function __destruct() { throw new Exception; } };
    $a = 1;
    return [0, $a];
}

function test2() {
    $x = test();
}

try {
    test2();
} catch (Exception $e) {
    echo "caught\n";
}

?>
--EXPECT--
caught
