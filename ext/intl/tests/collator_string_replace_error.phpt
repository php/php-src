--TEST--
Collator::replace (error)
--SKIPIF--
<?php if (!extension_loaded('intl')) echo 'skip'; ?>
--INI--
intl.error_level=0
intl.use_exceptions=0
--FILE--
<?php
$coll = new Collator(NULL);

var_dump($coll->replace('subject', 'search'));
var_dump($coll->replace('subject', 'search', 'replacement', $count, 'FAIL'));
?>
--EXPECTF--

Warning: Collator::replace() expects at least 3 parameters, 2 given in %s on line %d
bool(false)

Warning: Collator::replace() expects at most 4 parameters, 5 given in %s on line %d
bool(false)
