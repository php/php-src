--TEST--
testing /r modifier in preg_* functions
--SKIPIF--
<?php
if (PCRE_VERSION_MAJOR == 10 && PCRE_VERSION_MINOR < 43) {
    die("skip old pcre version");
}
?>
--FILE--
<?php
echo "SK substitute matching" . PHP_EOL;
var_dump(preg_match('/AskZ/iur', 'AskZ')); // match
var_dump(preg_match('/AskZ/iur', 'aSKz')); // match
var_dump(preg_match('/AskZ/iur', "A\u{17f}kZ")); // no match
var_dump(preg_match('/AskZ/iur', "As\u{212a}Z")); // no match
var_dump(preg_match('/AskZ/iu', 'AskZ')); // match
var_dump(preg_match('/AskZ/iu', 'aSKz')); // match
var_dump(preg_match('/AskZ/iu', "A\u{17f}kZ")); // match
var_dump(preg_match('/AskZ/iu', "As\u{212a}Z")); // match

echo "K substitute matching" . PHP_EOL;
var_dump(preg_match('/k/iu', "\u{212A}"));
var_dump(preg_match('/k/iur', "\u{212A}"));

echo "non-ASCII in expressions" . PHP_EOL;
var_dump(preg_match('/A\x{17f}\x{212a}Z/iu', 'AskZ')); // match
var_dump(preg_match('/A\x{17f}\x{212a}Z/iur', 'AskZ')); // no match

echo "Character sets" . PHP_EOL;
var_dump(preg_match('/[AskZ]+/iur', 'AskZ')); // match
var_dump(preg_match('/[AskZ]+/iur', 'aSKz')); // match
var_dump(preg_match('/[AskZ]+/iur', "A\u{17f}kZ")); // match
var_dump(preg_match('/[AskZ]+/iur', "As\u{212a}Z")); // match
var_dump(preg_match('/[AskZ]+/iu', 'AskZ')); // match
var_dump(preg_match('/[AskZ]+/iu', 'aSKz')); // match
var_dump(preg_match('/[AskZ]+/iu', "A\u{17f}kZ")); // match
var_dump(preg_match('/[AskZ]+/iu', "As\u{212a}Z")); // match

echo "non-ASCII in character sets" . PHP_EOL;
var_dump(preg_match('/[\x{17f}\x{212a}]+/iur', 'AskZ')); // no match
var_dump(preg_match('/[\x{17f}\x{212a}]+/iu', 'AskZ')); // match

echo "Meta characters and negate character sets". PHP_EOL;
var_dump(preg_match('/[^s]+/iur', "A\u{17f}Z")); // match
var_dump(preg_match('/[^s]+/iu', "A\u{17f}Z")); // match
var_dump(preg_match('/[^s]+/iu', "A\u{17f}Z")); // match
var_dump(preg_match('/[^k]+/iur', "A\u{212a}Z")); // match
var_dump(preg_match('/[^k]+/iu', "A\u{212a}Z")); // match
var_dump(preg_match('/[^sk]+/iur', "A\u{17f}\u{212a}Z")); // match
var_dump(preg_match('/[^sk]+/iu', "A\u{17f}\u{212a}Z")); // match
var_dump(preg_match('/[^\x{17f}]+/iur', "AsSZ")); // match
var_dump(preg_match('/[^\x{17f}]+/iu', "AsSZ")); // match

echo "Modifier used within the expression" . PHP_EOL;
var_dump(preg_match('/s(?r)s(?-r)s(?r:s)s/iu', "\u{17f}S\u{17f}S\u{17f}")); // match
var_dump(preg_match('/s(?r)s(?-r)s(?r:s)s/iu', "\u{17f}\u{17f}\u{17f}S\u{17f}")); // no match
var_dump(preg_match('/s(?r)s(?-r)s(?r:s)s/iu', "\u{17f}S\u{17f}\u{17f}\u{17f}")); // no match
var_dump(preg_match('/k(?^i)k/iur', "K\u{212a}")); // match
var_dump(preg_match('/k(?^i)k/iur', "\u{212a}\u{212a}")); // no match

echo "Done";
?>
--EXPECT--
SK substitute matching
int(1)
int(1)
int(0)
int(0)
int(1)
int(1)
int(1)
int(1)
K substitute matching
int(1)
int(0)
non-ASCII in expressions
int(1)
int(0)
Character sets
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
non-ASCII in character sets
int(0)
int(1)
Meta characters and negate character sets
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
Modifier used within the expression
int(1)
int(0)
int(0)
int(1)
int(0)
Done
