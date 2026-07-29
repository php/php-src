--TEST--
FILTER_SANITIZE_ENCODED percent-encodes 0xFF
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("\xFF", FILTER_SANITIZE_ENCODED));
var_dump(filter_var("\xFE\xFF\x00A", FILTER_SANITIZE_ENCODED));
?>
--EXPECT--
string(3) "%FF"
string(10) "%FE%FF%00A"
