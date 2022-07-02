--TEST--
IntlBreakIterator::previous(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans');

var_dump($bi->last());
var_dump($bi->previous());
?>
--EXPECT--
int(13)
int(8)
