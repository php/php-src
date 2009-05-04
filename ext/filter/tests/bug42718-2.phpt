--TEST--
Bug #42718 - 2 (unsafe_raw filter not applied when configured as default filter)
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
magic_quotes_gpc=1
filter.default=unsafe_raw
filter.default_flags=
--GET--
a=1%00
--FILE--
<?php
echo ini_get('filter.default') . "\n";
echo ini_get('filter.default_flags') . "\n";
echo addcslashes($_GET['a'],"\0") . "\n";
?>
--EXPECT--
unsafe_raw

1\0
PHP Warning:  Directive 'magic_quotes_gpc' is deprecated in PHP 5.3 and greater in Unknown on line 0
