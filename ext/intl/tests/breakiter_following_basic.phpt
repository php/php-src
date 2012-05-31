--TEST--
BreakIterator::following(): basic test
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = BreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans zoo bee');

var_dump($bi->following(5));
var_dump($bi->following(50));
var_dump($bi->following(-1));
?>
==DONE==
--EXPECT--
int(7)
int(-1)
int(0)
==DONE==