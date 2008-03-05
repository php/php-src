--TEST--
Bug #43301 (mb_ereg*_replace() crashes when replacement string is invalid PHP expression and 'e' option is used)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

$ptr = 'hello';

$txt = <<<doc
hello, I have got a cr*sh on you
doc;

echo mb_ereg_replace($ptr,'$1',$txt,'e');
?>
--EXPECTF--
Parse error: syntax error, unexpected T_LNUMBER, expecting T_VARIABLE or '$' in %s/bug43301.php(%d) : mbregex replace on line 1

Fatal error: mb_ereg_replace(): Failed evaluating code: 
$1 in %s/bug43301.php on line %d
