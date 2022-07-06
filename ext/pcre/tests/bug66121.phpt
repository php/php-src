--TEST--
Bug #66121 - UTF-8 lookbehinds match bytes instead of characters
--FILE--
<?php
// Sinhala characters
var_dump(preg_replace('/(?<!ක)/u', '*', 'ක'));
var_dump(preg_replace('/(?<!ක)/u', '*', 'ම'));
// English characters
var_dump(preg_replace('/(?<!k)/u', '*', 'k'));
var_dump(preg_replace('/(?<!k)/u', '*', 'm'));
// Sinhala characters
preg_match_all('/(?<!ක)/u', 'ම', $matches, PREG_OFFSET_CAPTURE);
var_dump($matches);
// invalid UTF-8
var_dump(preg_replace('/(?<!ක)/u', '*', "\xFCක"));
var_dump(preg_replace('/(?<!ක)/u', '*', "ක\xFC"));
var_dump(preg_match_all('/(?<!ක)/u', "\xFCම", $matches, PREG_OFFSET_CAPTURE));
var_dump(preg_match_all('/(?<!ක)/u', "\xFCම", $matches, PREG_OFFSET_CAPTURE));
?>
--EXPECT--
string(4) "*ක"
string(5) "*ම*"
string(2) "*k"
string(3) "*m*"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(0) ""
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      string(0) ""
      [1]=>
      int(3)
    }
  }
}
NULL
NULL
bool(false)
bool(false)
