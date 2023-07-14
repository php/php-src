--TEST--
Check that invalid UTF-8 strings are NOT marked as valid UTF-8
--EXTENSIONS--
zend_test
--FILE--
<?php
// Invalid 2 Octet Sequence
$non_utf8 = "\xc3\x28";

echo "Integer cast to string concatenated to invalid UTF-8:\n";
$i = 2563;
$s = (string) $i;
$s .= "\xc3\x28";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Float cast to string concatenated to invalid UTF-8:\n";
$f = 26.7;
$s = (string) $f;
$s .= "\xc3\x28";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$f = 2e100;
$s = (string) $f;
$s .= "\xc3\x28";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 strings in variables, followed by concatenation of invalid UTF-8:\n";
$s1 = "f";
$s2 = "o";
$s = $s1 . $s2;
$s = $s . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Multiple concatenation known valid UTF-8 strings in variables, followed by concatenation of invalid UTF-8:\n";
$s1 = "f";
$s2 = "o";
$s = $s1 . $s2 . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 with invalid UTF-8 in assignment:\n";
$s = "f" . "\xc3\x28";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

// The "foo" string matches with a "Foo" class which is registered by the zend_test extension.
// That class name does not have the "valid UTF-8" flag because class names in general
// don't have to be UTF-8. As the "foo" string here goes through the interning logic,
// the string gets replaced by the "foo" string from the class, which does
// not have the "valid UTF-8" flag. We therefore choose a different test case: "fxo".
// The previous "foo" test case works because it is not interned.
echo "Multiple concatenation known valid UTF-8 and invalid UTF-8 in assignment:\n";
$s = "f" . "o" . "\xc3\x28";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 string with empty string in variables, followed by concatenation of invalid UTF-8:\n";
$s1 = "f";
$s2 = "";
$s = $s1 . $s2;
$s = $s . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$s1 = "f";
$s2 = "";
$s = $s2 . $s1;
$s = $s . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 string with empty string in assignment, followed by concatenation of invalid UTF-8:\n";
$s = "f" . "";
$s = $s . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$s = "" . "f";
$s = $s . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation in loop:\n";
const COPY_TIMES = 10_000;
$string = "a";

$string_concat = $string;
for ($i = 1; $i < COPY_TIMES; $i++) {
    $string_concat = $string_concat . $string;
}
$string_concat = $string_concat . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));

echo "Concatenation in loop (compound assignment):\n";
$string = "a";

$string_concat = $string;
for ($i = 1; $i < COPY_TIMES; $i++) {
    $string_concat .= $string;
}
$string_concat = $string_concat . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));

echo "Concatenation of objects:\n";
class ToString {
    public function __toString() : string{
        return "z";
    }
}
$o = new ToString();
$s = $o . $o;
$s = $s . $non_utf8;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Rope concat:\n";
$foo = 'f';
$bar = "\xc3";
$baz = 'a';
$rope = "$foo$bar$baz";
var_dump(zend_test_is_string_marked_as_valid_utf8($rope));
?>
--EXPECT--
Integer cast to string concatenated to invalid UTF-8:
bool(false)
Float cast to string concatenated to invalid UTF-8:
bool(false)
bool(false)
Concatenation known valid UTF-8 strings in variables, followed by concatenation of invalid UTF-8:
bool(false)
Multiple concatenation known valid UTF-8 strings in variables, followed by concatenation of invalid UTF-8:
bool(false)
Concatenation known valid UTF-8 with invalid UTF-8 in assignment:
bool(false)
Multiple concatenation known valid UTF-8 and invalid UTF-8 in assignment:
bool(false)
Concatenation known valid UTF-8 string with empty string in variables, followed by concatenation of invalid UTF-8:
bool(false)
bool(false)
Concatenation known valid UTF-8 string with empty string in assignment, followed by concatenation of invalid UTF-8:
bool(false)
bool(false)
Concatenation in loop:
bool(false)
Concatenation in loop (compound assignment):
bool(false)
Concatenation of objects:
bool(false)
Rope concat:
bool(false)
