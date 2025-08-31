--TEST--
Test iconv_strlen() function : basic functionality
--EXTENSIONS--
iconv
--FILE--
<?php
/*
 * Test basic functionality of iconv_strlen()
 */

echo "*** Testing iconv_strlen() : basic functionality***\n";

$string_ascii = 'abc def';
//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII String --\n";
var_dump(iconv_strlen($string_ascii));

echo "\n-- Multibyte String --\n";
var_dump(iconv_strlen($string_mb, 'UTF-8'));
?>
--EXPECT--
*** Testing iconv_strlen() : basic functionality***

-- ASCII String --
int(7)

-- Multibyte String --
int(21)
