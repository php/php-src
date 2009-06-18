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

// Using deprectated setlocale() in PHP6. The test needs to be changed
// when there is an alternative available.

chdir(dirname(__FILE__));
setlocale(LC_MESSAGES, 'en_US.UTF-8');
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain('dgettextTest', './locale');
bindtextdomain('dgettextTest_switch', './locale');
textdomain('dgettextTest');

var_dump(gettext('item'));
var_dump(dgettext('dgettextTest_switch', 'item'));
var_dump(gettext('item'));
?>
--EXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s.php on line %d

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s.php on line %d
string(7) "Produkt"
string(16) "Produkt_switched"
string(7) "Produkt"
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-09
