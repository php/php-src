--TEST--
Uninitialized run-time cache when resolving default values
--FILE--
<?php

class Test {
    public static function method($a = FOO, $b = 1) {
        echo "a = $a, b = $b\n";
    }
}

define('FOO', 42);
call_user_func(['Test', 'method'], b: 0);

?>
--EXPECT--
a = 42, b = 0
