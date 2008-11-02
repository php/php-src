--TEST--
preg_replace() and invalid UTF8 offset
--FILE--
<?php

$string = "\xc3\xa9 uma string utf8 bem formada";

var_dump(preg_match('~.*~u', $string, $m, 0, 1));
var_dump($m);
var_dump(preg_last_error() == PREG_BAD_UTF8_OFFSET_ERROR);

var_dump(preg_match('~.*~u', $string, $m, 0, 2));
var_dump($m);
var_dump(preg_last_error() == PREG_NO_ERROR);

echo "Done\n";
?>
--EXPECT--
int(0)
array(0) {
}
bool(true)
int(1)
array(1) {
  [0]=>
  string(28) " uma string utf8 bem formada"
}
bool(true)
Done
