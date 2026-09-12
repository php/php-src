--TEST--
PREG_THROW_ON_ERROR: the flag is opt-in and does not change behavior on success
--FILE--
<?php

$bad = "\xff";

var_dump(preg_match('//u', $bad, $m));
var_dump(preg_last_error() === PREG_BAD_UTF8_ERROR);
echo preg_last_error_msg(), "\n";

var_dump(preg_match('/\d+/', 'abc123', $m, PREG_THROW_ON_ERROR));
echo $m[0], "\n";
var_dump(preg_last_error() === PREG_NO_ERROR);

var_dump(preg_match('/\d+/', 'abc123', $m, PREG_OFFSET_CAPTURE | PREG_THROW_ON_ERROR));
var_dump($m[0]);

?>
--EXPECT--
bool(false)
bool(true)
Malformed UTF-8 characters, possibly incorrectly encoded
int(1)
123
bool(true)
int(1)
array(2) {
  [0]=>
  string(3) "123"
  [1]=>
  int(3)
}
