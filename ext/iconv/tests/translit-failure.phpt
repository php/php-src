--TEST--
Translit failture
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
// Should be ok.
// Content from file is from libiconv testkit. Tested both
// with a string as an implode, no difference.
// if at some point internal encoding changes, set correct one
// in INI section or use file 'TranslitFail1.ISO-8859-1'.

$test = 'Écrit par %s.';
if (!iconv("ISO-8859-1", "ASCII//TRANSLIT", $test))
	echo 'wrong is right';
?>
--EXPECT--
wrong is right
