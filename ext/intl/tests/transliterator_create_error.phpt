--TEST--
Transliterator::create (error)
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(Transliterator::create("inexistent id"));
echo intl_get_error_message(), "\n";
var_dump(Transliterator::create("bad UTF-8 \x8F"));
echo intl_get_error_message(), "\n";

?>
--EXPECT--
NULL
Transliterator::create(): unable to open ICU transliterator with id "inexistent id": U_INVALID_ID
NULL
Transliterator::create(): String conversion of id to UTF-16 failed: U_INVALID_CHAR_FOUND
