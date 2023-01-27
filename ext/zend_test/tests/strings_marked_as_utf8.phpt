--TEST--
Check that strings are marked as valid UTF-8
--EXTENSIONS--
zend_test
--FILE--
<?php
echo "Empty strings:\n";
$s = "";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Known strings:\n";
$s = "c";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Integer cast to string:\n";
$i = 2563;
$s = (string) $i;
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Float cast to string:\n";
$f = 26.7;
$s = (string) $f;
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$f = 2e100;
$s = (string) $f;
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 strings in variables:\n";
$s1 = "f";
$s2 = "o";
$s = $s1 . $s2;
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Multiple concatenation known valid UTF-8 strings in variables:\n";
$s1 = "f";
$s2 = "o";
$s = $s1 . $s2 . $s2;
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 in assignment:\n";
$s = "f" . "o";
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Multiple concatenation known valid UTF-8 in assignment:\n";
$s = "f" . "o" . "o";
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 string with empty string in variables:\n";
$s1 = "f";
$s2 = "";
$s = $s1 . $s2;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$s1 = "f";
$s2 = "";
$s = $s2 . $s1;
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation known valid UTF-8 string with empty string in assignment:\n";
$s = "f" . "";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$s = "" . "f";
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

echo "Concatenation in loop:\n";
const COPY_TIMES = 10_000;
$string = "a";

$string_concat = $string;
for ($i = 1; $i < COPY_TIMES; $i++) {
    $string_concat = $string_concat . $string;
}
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));

echo "Concatenation in loop (compound assignment):\n";
$string = "a";

$string_concat = $string;
for ($i = 1; $i < COPY_TIMES; $i++) {
    $string_concat .= $string;
}
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));

echo "Concatenation of objects:\n";
class ToString {
    public function __toString() : string{
        return "z";
    }
}
$o = new ToString();
$s = $o . $o;
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));

?>
--EXPECT--
Empty strings:
bool(true)
Known strings:
bool(true)
Integer cast to string:
string(4) "2563"
bool(true)
Float cast to string:
string(4) "26.7"
bool(true)
string(8) "2.0E+100"
bool(true)
Concatenation known valid UTF-8 strings in variables:
string(2) "fo"
bool(false)
Multiple concatenation known valid UTF-8 strings in variables:
string(3) "foo"
bool(false)
Concatenation known valid UTF-8 in assignment:
string(2) "fo"
bool(false)
Multiple concatenation known valid UTF-8 in assignment:
string(3) "foo"
bool(false)
Concatenation known valid UTF-8 string with empty string in variables:
bool(true)
bool(true)
Concatenation known valid UTF-8 string with empty string in assignment:
bool(true)
bool(true)
Concatenation in loop:
bool(false)
Concatenation in loop (compound assignment):
bool(false)
Concatenation of objects:
string(2) "zz"
bool(false)
