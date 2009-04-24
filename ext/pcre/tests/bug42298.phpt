--TEST--
Bug #42298 (pcre gives bogus results with /u)
--INI--
unicode.runtime_encoding=utf-8
unicode.output_encoding=utf-8
--FILE--
<?php
$str = b"A\xc2\xa3BC";
preg_match_all('/\S\S/u', $str, $m);	var_dump($m);
preg_match_all('/\S{2}/u', $str, $m);	var_dump($m);

$str = b"A\xe2\x82\xac ";
preg_match_all('/\W\W/u', $str, $m);	var_dump($m);
preg_match_all('/\W{2}/u', $str, $m);	var_dump($m);

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    unicode(2) "A£"
    [1]=>
    unicode(2) "BC"
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    unicode(2) "A£"
    [1]=>
    unicode(2) "BC"
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    unicode(2) "€ "
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    unicode(2) "€ "
  }
}
