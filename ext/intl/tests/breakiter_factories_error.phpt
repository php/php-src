--TEST--
IntlBreakIterator factory methods: argument errors
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlBreakIterator::createWordInstance(array()));
var_dump(IntlBreakIterator::createSentenceInstance(NULL, 2));
var_dump(IntlBreakIterator::createCharacterInstance(NULL, 2));
var_dump(IntlBreakIterator::createTitleInstance(NULL, 2));
var_dump(IntlBreakIterator::createLineInstance(NULL, 2));
--EXPECTF--
Warning: IntlBreakIterator::createWordInstance() expects parameter 1 to be string, array given in %s on line %d

Warning: IntlBreakIterator::createWordInstance(): breakiter_create_word_instance: bad arguments in %s on line %d
NULL

Warning: IntlBreakIterator::createSentenceInstance() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::createSentenceInstance(): breakiter_create_sentence_instance: bad arguments in %s on line %d
NULL

Warning: IntlBreakIterator::createCharacterInstance() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::createCharacterInstance(): breakiter_create_character_instance: bad arguments in %s on line %d
NULL

Warning: IntlBreakIterator::createTitleInstance() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::createTitleInstance(): breakiter_create_title_instance: bad arguments in %s on line %d
NULL

Warning: IntlBreakIterator::createLineInstance() expects at most 1 parameter, 2 given in %s on line %d

Warning: IntlBreakIterator::createLineInstance(): breakiter_create_line_instance: bad arguments in %s on line %d
NULL
