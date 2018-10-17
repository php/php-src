--TEST--
Test preg_grep() function : basic functionality
--FILE--
<?php
/*
* proto array preg_grep(string regex, array input [, int flags])
* Function is implemented in ext/pcre/php_pcre.c
*/
$array = array('HTTP://WWW.EXAMPLE.COM', '/index.html', '/info/stat/', 'http://test.uk.com/index/html', '/display/dept.php');
var_dump($array);
var_dump(preg_grep('@^HTTP(.*?)\w{2,}$@i', $array)); //finds a string starting with http (regardless of case) (matches two)
var_dump(preg_grep('@(/\w+\.*/*)+@', $array)); //finds / followed by one or more of a-z, A-Z and 0-9, followed by zero or more . followed by zero or more / all more than once. (matches all)
var_dump(preg_grep('@^http://[^w]{3}.*$@i', $array)); //finds http:// (at the beginning of a string) not followed by 3 characters that aren't w's then anything to the end of the sttring (matches one)
var_dump(preg_grep('@.*?\.co\.uk$@i', $array)); //finds any address ending in .co.uk (matches none)
var_dump(preg_grep('@^HTTP(.*?)\w{2,}$@i', $array, PREG_GREP_INVERT)); //same as first example but the array created contains everything that is NOT matched but the regex (matches three)

?>
--EXPECT--
array(5) {
  [0]=>
  string(22) "HTTP://WWW.EXAMPLE.COM"
  [1]=>
  string(11) "/index.html"
  [2]=>
  string(11) "/info/stat/"
  [3]=>
  string(29) "http://test.uk.com/index/html"
  [4]=>
  string(17) "/display/dept.php"
}
array(2) {
  [0]=>
  string(22) "HTTP://WWW.EXAMPLE.COM"
  [3]=>
  string(29) "http://test.uk.com/index/html"
}
array(5) {
  [0]=>
  string(22) "HTTP://WWW.EXAMPLE.COM"
  [1]=>
  string(11) "/index.html"
  [2]=>
  string(11) "/info/stat/"
  [3]=>
  string(29) "http://test.uk.com/index/html"
  [4]=>
  string(17) "/display/dept.php"
}
array(1) {
  [3]=>
  string(29) "http://test.uk.com/index/html"
}
array(0) {
}
array(3) {
  [1]=>
  string(11) "/index.html"
  [2]=>
  string(11) "/info/stat/"
  [4]=>
  string(17) "/display/dept.php"
}
