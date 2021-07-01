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
--EXPECT--
strlen(): Passing null to parameter #1 ($string) of type string is deprecated
