--TEST--
strrchr() tests
--FILE--
<?php

var_dump(strrchr("", ""));
var_dump(strrchr("abc", ""));
var_dump(strrchr("", "abc"));
var_dump(strrchr("abc", "abc"));
var_dump(strrchr("test ".chr(0)." test", " "));
var_dump(strrchr("test".chr(0)."string", "t"));

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
unicode(3) "abc"
unicode(5) " test"
unicode(5) "tring"
Done
