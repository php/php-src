--TEST--
Exception handler should not be invoked for exit()
--FILE--
<?php

set_exception_handler(function($e) {
    var_dump($e);
});

exit("Exit\n");

?>
--EXPECT--
Exit
