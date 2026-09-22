--TEST--
GH-13097 (Anonymous class reference in trigger_error / thrown Exception)
--FILE--
<?php

$anonymous = new class(){};

throw new Exception(
    get_class($anonymous).' ...now you don\'t!',
    E_USER_ERROR
);

?>
--EXPECTF--
Fatal error: Uncaught Exception: class@anonymous%s ...now you don't! in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
