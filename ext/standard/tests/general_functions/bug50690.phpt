--TEST--
Bug #23650 (putenv() does not assign values when the value is one character)
--FILE--
<?php
putenv("foo=ab");
putenv("bar=c");
var_dump(getenv("foo"));
var_dump(getenv("bar"));
var_dump(getenv("thisvardoesnotexist"));
?>
--EXPECT--
string(2) "ab"
string(1) "c"
bool(false)
