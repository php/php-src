--TEST--
Bug #69203 (FILTER_FLAG_STRIP_HIGH doesn't strip ASCII 127)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("\x7f", FILTER_SANITIZE_STRING, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_var("\x7f", FILTER_UNSAFE_RAW, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_var("\x7f", FILTER_SANITIZE_ENCODED, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_var("\x7f", FILTER_SANITIZE_SPECIAL_CHARS, FILTER_FLAG_STRIP_HIGH));
?>
--EXPECTF--
Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(0) ""
string(0) ""
string(0) ""
string(0) ""
