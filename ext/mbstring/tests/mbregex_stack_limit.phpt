--TEST--
Test oniguruma stack limit
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$s = str_repeat(' ', 30000);

ini_set('mbstring.regex_stack_limit', 10000);
var_dump(mb_ereg('\\s+$', $s));

ini_set('mbstring.regex_stack_limit', 30000);
var_dump(mb_ereg('\\s+$', $s));

ini_set('mbstring.regex_stack_limit', 30001);
var_dump(mb_ereg('\\s+$', $s));

echo 'OK';
?>
--EXPECT--
bool(false)
bool(false)
int(1)
OK
