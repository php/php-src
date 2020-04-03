--TEST--
Bug #78434: Generator yields no items after valid() call
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
    // Comment out this line to get the correct result.
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
