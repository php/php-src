--TEST--
UUID::v5 throws ArgumentCountError if no arguments are passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    UUID::v5();
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
UUID::v5() expects exactly 2 parameters, 0 given
