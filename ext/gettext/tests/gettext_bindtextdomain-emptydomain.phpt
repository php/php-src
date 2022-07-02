--TEST--
Test if bindtextdomain() errors if the domain is empty.
--EXTENSIONS--
gettext
--FILE--
<?php

chdir(__DIR__);

try {
    bindtextdomain('', 'foobar');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
bindtextdomain(): Argument #1 ($domain) cannot be empty
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09
