--TEST--
Bug #78434: Generator skips first item after valid() call
--FILE--
<?php

$function = function () {
    yield 0;
};

$wrapper = function () use ($function) {
    $generator = $function();
    $generator->valid();
    yield from $generator;

    $generator = $function();
    $generator->valid();
    yield from $generator;
};

foreach ($wrapper() as $value) {
    echo $value, "\n";
}

?>
--EXPECT--
0
0
