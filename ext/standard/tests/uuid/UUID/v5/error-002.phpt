--TEST--
UUID::v5 throws ArgumentCountError if only one argument is passed
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    UUID::v5(UUID::Nil());
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Too few arguments to method UUID::v5(), 1 passed and exactly 2 expected
