--TEST--
grapheme() str[i]pos limits
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) die('skip');
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
var_dump(grapheme_stripos(1,1,2147483648));
var_dump(grapheme_strpos(1,1,2147483648));
--EXPECT--
bool(false)
bool(false)
