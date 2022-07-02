--TEST--
Bug #62082: Memory corruption in internal get_icu_disp_value_src_php()
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(locale_get_display_name(str_repeat("a", 300), null));
var_dump(locale_get_display_name(str_repeat("a", 512), null));
var_dump(locale_get_display_name(str_repeat("a", 600), null));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
