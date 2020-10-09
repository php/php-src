--TEST--
Bug #49354 (mb_strcut() cuts wrong length when offset is in the middle of a multibyte character)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$crap = 'AåBäCöDü';
var_dump(mb_strcut($crap, 0, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 1, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 2, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 3, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 12, 100, 'UTF-8'));
var_dump(mb_strcut($crap, 13, 100, 'UTF-8'));

?>
--EXPECT--
string(12) "AåBäCöDü"
string(11) "åBäCöDü"
string(11) "åBäCöDü"
string(9) "BäCöDü"
string(0) ""
string(0) ""
