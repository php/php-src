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

// The "foo" string matches with a "Foo" class which is registered by the zend_test extension.
// That class name does not have the "valid UTF-8" flag because class names in general
// don't have to be UTF-8. As the "foo" string here goes through the interning logic,
// the string gets replaced by the "foo" string from the class, which does
// not have the "valid UTF-8" flag. We therefore choose a different test case: "fxo".
// The previous "foo" test case works because it is not interned.
echo "Multiple concatenation known valid UTF-8 in assignment:\n";
$s = "f" . "o" . "o";
var_dump($s);
var_dump(zend_test_is_string_marked_as_valid_utf8($s));
$s = "f" . "x" . "o";
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

echo "Rope concat:\n";
$foo = 'f';
$bar = 'b';
$baz = 'a';
$rope = "$foo$bar$baz";
var_dump(zend_test_is_string_marked_as_valid_utf8($rope));

echo "str_repeat:\n";
$string = "a";
$string_concat = str_repeat($string, 100);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string = "\xff";
$string_concat = str_repeat($string, 100);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));

echo "implode:\n";
$arr = ['a', 'b'];
$string_concat = implode('', $arr);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('|', $arr);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', ['c', ...$arr]);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', [...$arr, 'c']);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', ["\xff", ...$arr]);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', [...$arr, "\xff"]);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode("\xff", $arr);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', []);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode("\xff", []);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', ['a']);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode("\xff", ['a']);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));
$string_concat = implode('', [1, 1.0, 'a']);
var_dump(zend_test_is_string_marked_as_valid_utf8($string_concat));

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
bool(true)
Multiple concatenation known valid UTF-8 strings in variables:
string(3) "foo"
bool(true)
Concatenation known valid UTF-8 in assignment:
string(2) "fo"
bool(true)
Multiple concatenation known valid UTF-8 in assignment:
string(3) "foo"
bool(false)
string(3) "fxo"
bool(true)
Concatenation known valid UTF-8 string with empty string in variables:
bool(true)
bool(true)
Concatenation known valid UTF-8 string with empty string in assignment:
bool(true)
bool(true)
Concatenation in loop:
bool(true)
Concatenation in loop (compound assignment):
bool(true)
Concatenation of objects:
string(2) "zz"
bool(true)
Rope concat:
bool(true)
str_repeat:
bool(true)
bool(false)
implode:
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
