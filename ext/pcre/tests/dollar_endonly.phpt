--TEST--
D (PCRE_DOLLAR_ENDONLY) modifier
--FILE--
<?php

var_dump(preg_match_all('/^\S+.+$/', "aeiou\n", $m));
var_dump($m);

var_dump(preg_match_all('/^\S+.+$/D', "aeiou\n", $m));
var_dump($m);

var_dump(preg_match_all('/^\S+\s$/D', "aeiou\n", $m));
var_dump($m);

?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(5) "aeiou"
  }
}
int(0)
array(1) {
  [0]=>
  array(0) {
  }
}
int(1)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(6) "aeiou
"
  }
}
