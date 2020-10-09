--TEST--
Test if bindtextdomain() errors if the domain is empty.
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("skip gettext extension is not loaded.\n");
}
?>
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
