--TEST--
Arbitrary string interpolation with "{$:expr}"
--FILE--
<?php

const foo = 'bar';

function foo() {
    return 'bar';
}

echo "{$:foo()}", PHP_EOL;
echo "{$:foo}", PHP_EOL;
echo "{$:'f' . 'o' . 'o'}", PHP_EOL;
echo "{$:"f" . "o" . "o"}", PHP_EOL;
echo "{$:"{$:"foo"}"}", PHP_EOL;
echo "{$:1 + 2 + 3}", PHP_EOL;

?>
--EXPECT--
bar
bar
foo
foo
foo
6
