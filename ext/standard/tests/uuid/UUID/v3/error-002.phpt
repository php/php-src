--TEST--
UUID::v3 throws ArgumentCountError if only one argument is passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    UUID::v3(UUID::Nil());
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
UUID::v3() expects exactly 2 parameters, 1 given
