--TEST--
Test if dngettext() errors when you don't supply the correct params.
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("skip gettext extension is not loaded.\n");
}
if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
    die("skip en_US.UTF-8 locale not supported.");
}
--FILE--
<?php
chdir(dirname(__FILE__));
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain('dngettextTest', './locale');

var_dump(dngettext('dngettextTest', 'item', 'items'));
--EXPECTF--
Warning: dngettext() expects exactly 4 parameters, 3 given in %s on line %d
NULL
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09