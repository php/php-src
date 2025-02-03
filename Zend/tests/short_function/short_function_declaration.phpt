--TEST--
Short function declaration
--FILE--
<?php
function foo(): void = echo 123;
var_dump(foo()); // NULL

function bar(): int = return 456;
echo bar() . PHP_EOL;

// function buz(): int = 456; // TODO: must be the same as return 456;
// echo buz() . PHP_EOL;

function compare(int $what, int $with): string = switch(true) {
    case $what > $with:
        return "greater";
        break;
    case $what < $with:
        return "less";
        break;
    case $what == $with:
        return "equals";
        break;
    default:
        throw new Exception("Unreachable statement");
    }
var_dump(compare(1, 2));
var_dump(compare(20, 10));
var_dump(compare(5, 5));

echo "done";
?>
--EXPECT--
123NULL
456
string(4) "less"
string(7) "greater"
string(6) "equals"
done