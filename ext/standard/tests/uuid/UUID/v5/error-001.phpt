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
Too few arguments to method UUID::v5(), 0 passed and exactly 2 expected
