--TEST--
spoofchecker suspicious character checker
--SKIPIF--
<?php if(!extension_loaded('intl') || !class_exists("Spoofchecker")) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '57.1') >= 0)die('skip for ICU <= 57.1'); ?>
--FILE--
<?php

$url = "http://www.payp\xD0\xB0l.com";

$x = new Spoofchecker();
echo "paypal with Cyrillic spoof characters\n";
var_dump($x->isSuspicious($url));

echo "certain all-uppercase Latin sequences can be spoof of Greek\n";
var_dump($x->isSuspicious("NAPKIN PEZ"));
var_dump($x->isSuspicious("napkin pez"));
?>
--EXPECTF--
paypal with Cyrillic spoof characters
bool(true)
certain all-uppercase Latin sequences can be spoof of Greek
bool(true)
bool(false)
