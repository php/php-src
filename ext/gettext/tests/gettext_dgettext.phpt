--TEST--
Test dgettext() functionality
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("SKIP gettext extension is not loaded.\n");
}
if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
    die("SKIP en_US.UTF-8 locale not supported.");
}
--FILE--
<?php
chdir(__DIR__);
putenv('LC_MESSAGES=en_US.UTF-8');
setlocale(LC_MESSAGES, 'en_US.UTF-8');
putenv('LC_ALL=en_US.UTF-8');
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain('dgettextTest', './locale');
bindtextdomain('dgettextTest_switch', './locale');
textdomain('dgettextTest');

var_dump(gettext('item'));
var_dump(dgettext('dgettextTest_switch', 'item'));
var_dump(gettext('item'));
?>
--EXPECT--
string(7) "Produkt"
string(16) "Produkt_switched"
string(7) "Produkt"
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-09
