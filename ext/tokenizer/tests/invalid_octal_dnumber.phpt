--TEST--
Invalid octal number that overflows to double
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php
$token = token_get_all('<?php 0177777777777777777777787')[1];
echo token_name($token[0]), "\n";
echo $token[1], "\n";
// The tokenizer should only warn about lost precision for octal literals when valid
$token = token_get_all('<?php 0177777777777777777777777')[1];
echo $token[1], "\n";
?>
--EXPECTF--
T_DNUMBER
0177777777777777777777787

Warning: Saw imprecise float octal literal - the last 17 non-zero bits were truncated in %sinvalid_octal_dnumber.php on line 6
0177777777777777777777777
