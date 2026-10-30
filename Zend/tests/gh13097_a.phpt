--TEST--
GH-13097 (Anonymous class reference in trigger_error / thrown Exception)
--FILE--
<?php

$anonymous = new class(){};

trigger_error(
    get_class($anonymous).' ...now you don\'t!',
    E_USER_ERROR
);

?>
--EXPECTF--
Deprecated: Passing E_USER_ERROR to trigger_error() is deprecated since 8.4, throw an exception or call exit with a string message instead in %s on line %d

Fatal error: class@anonymous%s ...now you don't! in %s on line %d
