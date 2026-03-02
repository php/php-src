--TEST--
Closure 018: Assigning lambda to static var and returning by ref
--FILE--
<?php

class foo {
    public function test(&$x) {
        static $lambda;
        $lambda = function &() use (&$x) {
            return $x = $x * $x;
        };
        return $lambda();
    }
}

$test = new foo;

$y = 2;
var_dump($test->test($y));
var_dump($x = $test->test($y));
var_dump($y, $x);

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %sclosure_018.php on line 7
int(4)

Notice: Only variable references should be returned by reference in %sclosure_018.php on line 7
int(16)
int(16)
int(16)
