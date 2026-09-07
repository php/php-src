--TEST--
Test if bindtextdomain() errors if the domain is empty.
--EXTENSIONS--
gettext
--FILE--
<?php

chdir(__DIR__);

try {
    bindtextdomain('', 'foobar');
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bindtextdomain(): Argument #1 ($domain) must not be empty
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09
