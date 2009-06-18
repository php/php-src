--TEST--
Test if bindtextdomain() errors if the domain is empty.
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("skip gettext extension is not loaded.\n");
}
--FILE--
<?php


chdir(dirname(__FILE__));
bindtextdomain('', 'foobar');
--EXPECTF--
Warning: bindtextdomain(): the first parameter must not be empty in %s.php on line %d
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09
