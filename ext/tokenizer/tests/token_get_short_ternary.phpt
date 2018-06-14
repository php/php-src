--TEST--
Test T_SHORT_TERNARY token
--SKIPIF--
<?php if (!extension_loaded('tokenizer')) die('skip tokenizer extension not loaded'); ?>
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
$tokens = token_get_all('<?php ?:');

var_dump(token_name($tokens[1][0]) === 'T_SHORT_TERNARY');
?>
--EXPECT--
bool(true)
