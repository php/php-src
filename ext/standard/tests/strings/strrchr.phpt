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
Warning: strrchr(): Empty needle in /mnt/c/Dev/php-src/ext/standard/tests/strings/strrchr.php on line 3
bool(false)

Warning: strrchr(): Empty needle in /mnt/c/Dev/php-src/ext/standard/tests/strings/strrchr.php on line 4
bool(false)
bool(false)
string(3) "abc"
string(5) " test"
string(5) "tring"
Done
