--TEST--
Invalid octal number that overflows to double
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php
echo token_name(token_get_all('<?php 0177777777777777777777787')[1][0]), "\n";
?>
--EXPECT--
T_DNUMBER
