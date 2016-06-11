--TEST--
Bug #60909 (custom error handler throwing Exception + fatal error = no shutdown function).
--FILE--
<?php
register_shutdown_function(function(){echo("\n\n!!!shutdown!!!\n\n");});
set_error_handler(function($errno, $errstr, $errfile, $errline){throw new Exception("Foo");});

class Bad {
    public function __toString() {
        throw new Exception('Oops, I cannot do this');
    }
}

$bad = new Bad();
echo "$bad";
--EXPECTF--
Fatal error: Method Bad::__toString() must not throw an exception, caught Exception: Oops, I cannot do this in %sbug60909_2.php on line %d


!!!shutdown!!!
