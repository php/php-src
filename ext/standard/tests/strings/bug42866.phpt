--TEST--
Bug #42866 (str_split() returns extra char when given string size is not multiple of length)
--FILE--
<?php
$str = 'Testing str_split()';
$split_length = 5;
var_dump( str_split($str, $split_length) );
?>
--EXPECT--
array(4) {
  [0]=>
  string(5) "Testi"
  [1]=>
  string(5) "ng st"
  [2]=>
  string(5) "r_spl"
  [3]=>
  string(4) "it()"
}
--UEXPECT--
array(4) {
  [0]=>
  unicode(5) "Testi"
  [1]=>
  unicode(5) "ng st"
  [2]=>
  unicode(5) "r_spl"
  [3]=>
  unicode(4) "it()"
}
