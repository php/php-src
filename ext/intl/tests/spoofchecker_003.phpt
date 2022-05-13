--TEST--
spoofchecker with locale settings
--EXTENSIONS--
intl
--SKIPIF--
<?php if(!class_exists("Spoofchecker")) print 'skip'; ?>
--FILE--
<?php

$korean = "\xED\x95\x9C" . "\xEA\xB5\xAD" . "\xEB\xA7\x90";

$x = new Spoofchecker();
echo "Is suspcious, en_US\n";

$x->setAllowedLocales('en_US');
var_dump($x->isSuspicious($korean));

echo "Is suspcious, ko_KR\n";

$x->setAllowedLocales('en_US, ko_KR');
var_dump($x->isSuspicious($korean));
?>
--EXPECT--
Is suspcious, en_US
bool(true)
Is suspcious, ko_KR
bool(false)
