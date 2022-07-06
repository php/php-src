--TEST--
It's possible to invoke a generator dynamically
--FILE--
<?php

function gen($foo, $bar) {
    yield $foo;
    yield $bar;
}

$gen = call_user_func('gen', 'bar', 'foo');
foreach ($gen as $value) {
    var_dump($value);
}

?>
--EXPECT--
string(3) "bar"
string(3) "foo"
