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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
takesInt(): Argument #1 ($x) must be of type int, string given, called in %s on line %d
