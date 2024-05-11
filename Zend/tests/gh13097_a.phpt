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
Fatal error: class@anonymous%s ...now you don't! in %s on line %d
