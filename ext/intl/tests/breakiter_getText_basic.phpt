--TEST--
IntlBreakIterator::getText(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = IntlBreakIterator::createWordInstance('pt');
var_dump($bi->getText());
$bi->setText('foo bar');
var_dump($bi->getText());
?>
--EXPECT--
NULL
string(7) "foo bar"
