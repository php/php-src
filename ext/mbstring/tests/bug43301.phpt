--TEST--
Bug #43301 (mb_ereg*_replace() crashes when replacement string is invalid PHP expression and 'e' option is used)
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php

$ptr = 'hello';

$txt = <<<doc
hello, I have got a cr*sh on you
doc;

echo mb_ereg_replace($ptr,'$1',$txt,'e');

?>
--EXPECTF--
Deprecated: mb_ereg_replace(): The 'e' option is deprecated, use mb_ereg_replace_callback instead in %s%ebug43301.php on line %d

Fatal error: Uncaught ParseError: syntax error, unexpected '1' (T_LNUMBER), expecting variable (T_VARIABLE) or '{' or '$' in %sbug43301.php(%d) : mbregex replace:1
Stack trace:
#0 %sbug43301.php(%d): mb_ereg_replace('hello', '$1', 'hello, I have g...', 'e')
#1 {main}

Next Error: Failed evaluating code: 
$1 in %sbug43301.php:%d
Stack trace:
#0 %sbug43301.php(%d): mb_ereg_replace('hello', '$1', 'hello, I have g...', 'e')
#1 {main}
  thrown in %sbug43301.php on line %d
