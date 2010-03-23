--TEST--
Bug #42298 (pcre gives bogus results with /u)
--FILE--
<?php
$str = "A\xc2\xa3BC";
preg_match_all('/\S\S/u', $str, $m);	var_dump($m);
preg_match_all('/\S{2}/u', $str, $m);	var_dump($m);

$str = "A\xe2\x82\xac ";
preg_match_all('/\W\W/u', $str, $m);	var_dump($m);
preg_match_all('/\W{2}/u', $str, $m);	var_dump($m);

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "A£"
    [1]=>
    string(2) "BC"
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "A£"
    [1]=>
    string(2) "BC"
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(4) "€ "
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(4) "€ "
  }
}
