--TEST--
Translit failure
--SKIPIF--
<?php
include('skipif.inc');
( ICONV_IMPL != "libiconv" ) and die("skip ICONV_IMPL != \"libiconv\""); 
?>
--INI--
error_reporting=2039
--FILE--
<?php
/* include('test.inc'); */
// Should be ok.
// Content from file is from libiconv testkit. Tested both
// with a string as an implode, no difference.
// if at some point internal encoding changes, set correct one
// in INI section or use file 'TranslitFail1.ISO-8859-1'.

set_time_limit(5);
/*
 * The bug (fixed in libiconv 1.8) was confirmed that iconv goes into an
 * infinite loop when ASCII//TRANSLIT is performed. We should stop it in
 * some time.
 */

$test = 'Écrit par %s.';

var_dump(iconv("ISO-8859-1", "ASCII//TRANSLIT", $test));
?>
--EXPECT--
string(14) "'Ecrit par %s."
