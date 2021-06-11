--TEST--
spoofchecker suspicious character checker
--EXTENSIONS--
intl
--SKIPIF--
<?php if(!class_exists("Spoofchecker")) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '57.1') < 0) die('skip for ICU >= 58.1'); ?>
--FILE--
<?php

echo "paypal with Cyrillic spoof characters\n";
$x = new Spoofchecker();
var_dump($x->isSuspicious("http://www.payp\u{0430}l.com"));
var_dump($x->isSuspicious("\u{041F}aypal.com"));

echo "certain all-uppercase Latin sequences can be spoof of Greek\n";
$x = new Spoofchecker();
$x->setAllowedLocales("gr_GR");
var_dump($x->isSuspicious("NAPKIN PEZ"));
var_dump($x->isSuspicious("napkin pez"));
?>
--EXPECT--
paypal with Cyrillic spoof characters
bool(true)
bool(true)
certain all-uppercase Latin sequences can be spoof of Greek
bool(true)
bool(true)
