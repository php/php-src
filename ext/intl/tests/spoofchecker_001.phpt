--TEST--
spoofchecker suspicious character checker
--SKIPIF--
<?php if(!extension_loaded('intl') || !class_exists("Spoofchecker")) print 'skip'; ?>
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
