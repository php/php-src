--TEST--
A function and a class with the same name work as expected
--FILE--
<?php

function Something(): string
{
    return 'Another';
}

class Something {}

class Another {}

echo Something() . PHP_EOL;
var_dump(new Something());
var_dump(new (Something()));

?>
--EXPECT--
Another
object(Something)#1 (0) {
}
object(Another)#1 (0) {
}
