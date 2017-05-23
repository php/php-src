--TEST--
UUID::v3 throws ArgumentCountError if no arguments are passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    UUID::v3();
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Too few arguments to method UUID::v3(), 0 passed and exactly 2 expected
