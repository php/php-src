--TEST--
strlen() null deprecation warning promoted to exception
--FILE--
<?php

set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});
try {
    strlen(null);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Fatal error: Uncaught Exception: strlen(): Passing null to parameter #1 ($string) of type string is deprecated in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(%s)
#1 {main}
  thrown in %s on line %d
