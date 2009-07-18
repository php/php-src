--TEST--
Bug #42718 (unsafe_raw filter not applied when configured as default filter)
--XFAIL--
FILTER_UNSAFE_RAW not applied when configured as default filter, even with flags
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
magic_quotes_gpc=0
filter.default=unsafe_raw
filter.default_flags=4
--GET--
a=1%00
--FILE--
<?php
echo ini_get('filter.default') . "\n";
echo ini_get('filter.default_flags') . "\n";
var_dump(FILTER_FLAG_STRIP_LOW == 4);
echo addcslashes($_GET['a'],"\0") . "\n";
?>
--EXPECT--
unsafe_raw
4
bool(true)
1
