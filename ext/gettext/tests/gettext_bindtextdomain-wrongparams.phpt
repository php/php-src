--TEST--
Test if bindtextdomain() errors if you don't supply enough parameters.
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("skip gettext extension is not loaded.\n");
}
--FILE--
<?php
chdir(dirname(__FILE__));
bindtextdomain('foobar');
bindtextdomain();
--EXPECTF--
Warning: bindtextdomain() expects exactly 2 parameters, 1 given in %s on line %d

Warning: bindtextdomain() expects exactly 2 parameters, 0 given in %s on line %d
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09
