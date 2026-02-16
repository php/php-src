--TEST--
PFA errors: Can not fetch default parameter value for Closure::__invoke()
--FILE--
<?php

$f = function ($a, $b, $c = null) {};
try {
    $g = $f->__invoke(0, 0, ?);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: Closure::__invoke(): Argument #3 ($c) must be passed explicitly, because the default value is not known
