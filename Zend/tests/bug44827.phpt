--TEST--
Bug #44827 (define() allows :: in constant names when it shouldn't)
--FILE--
<?php

try {
    define('foo::bar', 1);
}
catch (\Throwable $e) {
    echo $e;
}

?>
--EXPECTF--
ValueError: Class constants cannot be defined or redefined in %s:%d
Stack trace:
#0 %s(%d): define('foo::bar', 1)
#1 {main}
