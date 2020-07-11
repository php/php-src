--TEST--
IntlBreakIterator::getText(): basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$bi = IntlBreakIterator::createWordInstance('pt');
var_dump($bi->getText());
$bi->setText('foo bar');
var_dump($bi->getText());
--EXPECT--
NULL
string(7) "foo bar"
