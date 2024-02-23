--TEST--
exit() as function
--FILE--
<?php

function foo(callable $fn) {
    var_dump($fn);
}

$values = [
    'exit',
    'die',
    exit(...),
    die(...),
];

foreach ($values as $value) {
    foo($value);
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "...", expecting ")" in %s on line %d
