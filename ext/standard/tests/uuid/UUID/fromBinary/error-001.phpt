--TEST--
UUID::fromBinary throws ArgumentCountError if no arguments are passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    UUID::fromBinary();
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Too few arguments to method UUID::fromBinary(), 0 passed and exactly 1 expected
