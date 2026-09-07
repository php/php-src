--TEST--
preg_grep() returns false on match execution error (e.g. malformed UTF-8)
--FILE--
<?php
// preg_grep should return false when a match execution error occurs,
// consistent with preg_match behavior. See GH-11936.

// Test 1: preg_match returns false on malformed UTF-8 with /u modifier
var_dump(preg_match('/.*/u', hex2bin('ff')));
var_dump(preg_last_error() === PREG_BAD_UTF8_ERROR);

// Test 2: preg_grep should also return false (not an empty/partial array)
var_dump(preg_grep('/.*/u', [hex2bin('ff')]));
var_dump(preg_last_error() === PREG_BAD_UTF8_ERROR);

// Test 3: preg_grep with valid entries before the invalid one should
// return false, not a partial array
var_dump(preg_grep('/.*/u', ['foo', hex2bin('ff'), 'bar']));
var_dump(preg_last_error() === PREG_BAD_UTF8_ERROR);

// Test 4: preg_grep with PREG_GREP_INVERT should also return false on error
var_dump(preg_grep('/.*/u', [hex2bin('ff')], PREG_GREP_INVERT));
var_dump(preg_last_error() === PREG_BAD_UTF8_ERROR);

// Test 5: preg_grep without error still returns an array
var_dump(preg_grep('/.*/u', ['foo', 'bar']));
var_dump(preg_last_error() === PREG_NO_ERROR);
?>
--EXPECTF--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
bool(true)
