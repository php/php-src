--TEST--
GH-19719: Allow empty expressions before declare(strict_types)
--FILE--
<?php
// e.g some comments
?>
<?php

declare(strict_types=1);

function takesInt(int $x) {}

try {
    takesInt('42');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: takesInt(): Argument #1 ($x) must be of type int, string given, called in %s on line %d
