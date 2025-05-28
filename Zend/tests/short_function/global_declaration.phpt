--TEST--
Short global function declaration
--FILE--
<?php
function buz(): int => 123;
echo buz() . PHP_EOL;

function compare(int $what, int $with): string => match(true) {
     $what > $with => "greater",
     $what < $with => "less",
     $what == $with => "equals",
    default => throw new Exception("Unreachable statement"),
};

var_dump(compare(1, 2));
var_dump(compare(20, 10));
var_dump(compare(5, 5));

echo "done";
?>
--EXPECT--
123
string(4) "less"
string(7) "greater"
string(6) "equals"
done