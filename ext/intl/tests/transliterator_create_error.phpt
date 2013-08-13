--TEST--
Transliterator::create (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);
Transliterator::create("inexistant id");
echo intl_get_error_message(), "\n";
Transliterator::create("bad UTF-8 \x8F");
echo intl_get_error_message(), "\n";

echo "Done.\n";
--EXPECTF--
Warning: Transliterator::create(): transliterator_create: unable to open ICU transliterator with id "inexistant id" in %s on line %d
transliterator_create: unable to open ICU transliterator with id "inexistant id": U_INVALID_ID

Warning: Transliterator::create(): String conversion of id to UTF-16 failed in %s on line %d
String conversion of id to UTF-16 failed: U_INVALID_CHAR_FOUND
Done.
