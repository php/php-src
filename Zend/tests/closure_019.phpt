--TEST--
Closure 019: Calling lambda using $GLOBALS and global $var
--FILE--
<?php

$lambda = function &(&$x) {
    return $x = $x * $x;
};

function test() {
    global $lambda;

    $y = 3;
    var_dump($GLOBALS['lambda']($y));
    var_dump($lambda($y));
    var_dump($GLOBALS['lambda'](1));
}

test();

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %sclosure_019.php on line 4
int(9)

Notice: Only variable references should be returned by reference in %sclosure_019.php on line 4
int(81)

Fatal error: Uncaught Error: {closure}(): Argument #1 ($x) could not be passed by reference in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
  thrown in %s on line %d
