--TEST--
Bug #72241: get_icu_value_internal out-of-bounds read
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$var1=str_repeat("A", 1000);
$out = locale_get_primary_language($var1);
echo strlen($out) . PHP_EOL;
echo unpack('H*', $out)[1] . PHP_EOL;
--EXPECT--
0
