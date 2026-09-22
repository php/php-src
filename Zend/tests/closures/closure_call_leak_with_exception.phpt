--TEST--
Closure must not leak during a dynamic call interrupted by an exception
--FILE--
<?php

(function() {
    $closure = function($foo) { var_dump($foo); };
    $closure(yield);
})()->valid(); // start

?>
==DONE==
--EXPECT--
==DONE==
