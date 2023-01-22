--TEST--
Type inference 012: FETCH_DIM_UNSET
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    $closure = function() {return "string";};
    unset($x['b'][$closure()]['d']);
    $x = $arr;
    $arr = ['a' => $closure(), 'b' => [$closure() => []]];
    $x = $arr;
    unset($x['b'][$closure()]['d']);
    $x = $arr;
}

test();
?>
DONE
--EXPECTF--
Warning: Undefined variable $x (this will become an error in PHP 9.0) in %sinference_012.php on line 4

Warning: Undefined variable $arr (this will become an error in PHP 9.0) in %sinference_012.php on line 5
DONE
