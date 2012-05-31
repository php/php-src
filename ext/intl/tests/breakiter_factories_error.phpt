--TEST--
BreakIterator factory methods: argument errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(BreakIterator::createWordInstance(array()));
var_dump(BreakIterator::createSentenceInstance(NULL, 2));
var_dump(BreakIterator::createCharacterInstance(NULL, 2));
var_dump(BreakIterator::createTitleInstance(NULL, 2));
var_dump(BreakIterator::createLineInstance(NULL, 2));


--EXPECTF--

Warning: BreakIterator::createWordInstance() expects parameter 1 to be string, array given in %s on line %d

Warning: BreakIterator::createWordInstance(): breakiter_create_word_instance: bad arguments in %s on line %d
NULL

Warning: BreakIterator::createSentenceInstance() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::createSentenceInstance(): breakiter_create_sentence_instance: bad arguments in %s on line %d
NULL

Warning: BreakIterator::createCharacterInstance() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::createCharacterInstance(): breakiter_create_character_instance: bad arguments in %s on line %d
NULL

Warning: BreakIterator::createTitleInstance() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::createTitleInstance(): breakiter_create_title_instance: bad arguments in %s on line %d
NULL

Warning: BreakIterator::createLineInstance() expects exactly 1 parameter, 2 given in %s on line %d

Warning: BreakIterator::createLineInstance(): breakiter_create_line_instance: bad arguments in %s on line %d
NULL
