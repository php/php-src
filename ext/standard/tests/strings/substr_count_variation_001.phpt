--TEST--
Test substr_count() function (variation - 1)
--FILE--
<?php

echo "\n*** Testing possible variations ***\n";
echo "-- 3rd or 4th arg as string --\n";
$str = "this is a string";
var_dump( substr_count($str, "t", "5") );
var_dump( substr_count($str, "t", "5", "10") );

echo "\n-- overlapped substrings --\n";
var_dump( substr_count("abcabcabcabcabc", "abca") );
var_dump( substr_count("abcabcabcabcabc", "abca", 2) );

echo "\n-- complex strings containing other than 7-bit chars --\n";
$str = chr(128).chr(129).chr(128).chr(256).chr(255).chr(254).chr(255);
var_dump(substr_count($str, chr(128)));
var_dump(substr_count($str, chr(255)));
var_dump(substr_count($str, chr(256)));

echo "\n-- heredoc string --\n";
$string = <<<EOD
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
acdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
acdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;
var_dump(substr_count($string, "abcd"));
var_dump(substr_count($string, "1234"));

echo "\n-- heredoc null string --\n";
$str = <<<EOD
EOD;
var_dump(substr_count($str, "\0"));
var_dump(substr_count($str, "\x000"));
var_dump(substr_count($str, "0"));

echo "Done\n";

?>
--EXPECT--
*** Testing possible variations ***
-- 3rd or 4th arg as string --
int(1)
int(1)

-- overlapped substrings --
int(2)
int(2)

-- complex strings containing other than 7-bit chars --
int(2)
int(2)
int(1)

-- heredoc string --
int(14)
int(16)

-- heredoc null string --
int(0)
int(0)
int(0)
Done
