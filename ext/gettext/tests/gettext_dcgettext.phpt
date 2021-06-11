--TEST--
Test dcgettext() functionality
--EXTENSIONS--
gettext
--SKIPIF--
<?php

if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
    die("skip en_US.UTF-8 locale not supported.");
}
--FILE--
<?php
chdir(__DIR__);
putenv('LC_MESSAGES=en_US.UTF-8');
setlocale(LC_MESSAGES, 'en_US.UTF-8');
putenv('LC_ALL=en_US.UTF-8');
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain('dngettextTest', './locale');

var_dump(dcgettext('dngettextTest', 'item', LC_CTYPE));
var_dump(dcgettext('dngettextTest', 'item', LC_MESSAGES));
?>
--EXPECT--
string(8) "cProdukt"
string(7) "Produkt"
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
