--TEST--
Test mb_strstr() function : basic functionality
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "*** Testing mb_strstr() : basic functionality ***\n";

mb_internal_encoding('UTF-8');

$string_ascii = 'abc def';
//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- ASCII string: needle exists --\n";
var_dump(bin2hex(mb_strstr($string_ascii, 'd', false, 'ISO-8859-1')));
var_dump(bin2hex(mb_strstr($string_ascii, 'd')));
var_dump(bin2hex(mb_strstr($string_ascii, 'd', true)));


echo "\n-- ASCII string: needle doesn't exist --\n";
var_dump(mb_strstr($string_ascii, '123'));

echo "\n-- Multibyte string: needle exists --\n";
$needle1 = base64_decode('5pel5pys6Kqe');
var_dump(bin2hex(mb_strstr($string_mb, $needle1)));
var_dump(bin2hex(mb_strstr($string_mb, $needle1, false, 'utf-8')));
var_dump(bin2hex(mb_strstr($string_mb, $needle1, true)));


echo "\n-- Multibyte string: needle doesn't exist --\n";
$needle2 = base64_decode('44GT44KT44Gr44Gh44Gv44CB5LiW55WM');
var_dump(mb_strstr($string_mb, $needle2));

?>
--EXPECT--
*** Testing mb_strstr() : basic functionality ***

-- ASCII string: needle exists --
string(6) "646566"
string(6) "646566"
string(8) "61626320"

-- ASCII string: needle doesn't exist --
bool(false)

-- Multibyte string: needle exists --
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(0) ""

-- Multibyte string: needle doesn't exist --
bool(false)
