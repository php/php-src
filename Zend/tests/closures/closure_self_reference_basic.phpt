--TEST--
Testing Closure self-reference functionality: Basic
--FILE--
<?php

$factorial = function(int $num) as $factorial {
    if($num > 1) {
        return $num + $factorial($num - 1);
    }
    return $num;
};

echo "4! = ", $factorial(4), PHP_EOL;

$dynamic = function(bool $root = false) as $test use(&$dynamic) {
    if ($root) {
        $dynamic = function() {
            return "Hello";
        };
        return $test();
    } else {
        return "Hi";
    }
};

echo $dynamic(true), PHP_EOL;
echo $dynamic(true), PHP_EOL;

echo (function() as $fn : string {
    echo get_class($fn), PHP_EOL;
    return "Bye";
})(), PHP_EOL;

?>
--EXPECTF--
4! = 10
Hi
Hello
Closure
Bye
