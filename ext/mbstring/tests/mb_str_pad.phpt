--TEST--
mb_str_pad()
--EXTENSIONS--
mbstring
--FILE--
<?php

echo "--- Error conditions ---\n";
try {
    var_dump(mb_str_pad('▶▶', 6, '', STR_PAD_RIGHT));
} catch (ValueError $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(mb_str_pad('▶▶', 6, '', STR_PAD_LEFT));
} catch (ValueError $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(mb_str_pad('▶▶', 6, '', STR_PAD_BOTH));
} catch (ValueError $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(mb_str_pad('▶▶', 6, ' ', 123456));
} catch (ValueError $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(mb_str_pad('▶▶', 6, ' ', STR_PAD_BOTH, 'unexisting'));
} catch (ValueError $e) {
    var_dump($e->getMessage());
}

echo "--- Simple ASCII strings ---\n";
var_dump(mb_str_pad('Hello', 7, '+-', STR_PAD_BOTH));
var_dump(mb_str_pad('World', 10, '+-', STR_PAD_BOTH));
var_dump(mb_str_pad('Hello', 7, '+-', STR_PAD_LEFT));
var_dump(mb_str_pad('World', 10, '+-', STR_PAD_LEFT));
var_dump(mb_str_pad('Hello', 7, '+-', STR_PAD_RIGHT));
var_dump(mb_str_pad('World', 10, '+-', STR_PAD_RIGHT));

echo "--- Edge cases pad length ---\n";
var_dump(mb_str_pad('▶▶', 2, ' ', STR_PAD_BOTH));
var_dump(mb_str_pad('▶▶', 1, ' ', STR_PAD_BOTH));
var_dump(mb_str_pad('▶▶', 0, ' ', STR_PAD_BOTH));
var_dump(mb_str_pad('▶▶', -1, ' ', STR_PAD_BOTH));

echo "--- No pad string ---\n";
var_dump(mb_str_pad('▶▶', 4));
var_dump(mb_str_pad('▶▶', 3));
var_dump(mb_str_pad('▶▶', 2));
var_dump(mb_str_pad('▶▶', 1));

echo "--- Empty input string ---\n";
var_dump(mb_str_pad('', 2, ' ', STR_PAD_BOTH));
var_dump(mb_str_pad('', 1, ' ', STR_PAD_BOTH));
var_dump(mb_str_pad('', 0, ' ', STR_PAD_BOTH));
var_dump(mb_str_pad('', -1, ' ', STR_PAD_BOTH));

echo "--- No default argument ---\n";
var_dump(mb_str_pad('▶▶', 6, pad_type: STR_PAD_RIGHT));
var_dump(mb_str_pad('▶▶', 6, pad_type: STR_PAD_LEFT));
var_dump(mb_str_pad('▶▶', 6, pad_type: STR_PAD_BOTH));

echo "--- UTF-8 emojis ---\n";
for ($i = 6; $i > 0; $i--) {
    var_dump(mb_str_pad('▶▶', $i, '❤❓❇', STR_PAD_RIGHT));
    var_dump(mb_str_pad('▶▶', $i, '❤❓❇', STR_PAD_LEFT));
    var_dump(mb_str_pad('▶▶', $i, '❤❓❇', STR_PAD_BOTH));
}

echo "--- UTF-8, 32, 7 test ---\n";

// Taken from mb_substr.phpt
$utf8 = "Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь";
$utf32 = mb_convert_encoding($utf8, 'UTF-32', 'UTF-8');
$utf7 = mb_convert_encoding($utf8, 'UTF-7', 'UTF-8');
$tests = ["UTF-8" => $utf8, "UTF-32" => $utf32, "UTF-7" => $utf7];

foreach ($tests as $encoding => $test) {
    $pad_str = mb_convert_encoding('▶▶', $encoding, 'UTF-8');
    var_dump(mb_convert_encoding(mb_str_pad($test, 44, $pad_str, STR_PAD_RIGHT, $encoding), 'UTF-8', $encoding));
    var_dump(mb_convert_encoding(mb_str_pad($test, 44, $pad_str, STR_PAD_LEFT, $encoding), 'UTF-8', $encoding));
    var_dump(mb_convert_encoding(mb_str_pad($test, 44, $pad_str, STR_PAD_BOTH, $encoding), 'UTF-8', $encoding));
}
?>
--EXPECT--
--- Error conditions ---
string(66) "mb_str_pad(): Argument #3 ($pad_string) must be a non-empty string"
string(66) "mb_str_pad(): Argument #3 ($pad_string) must be a non-empty string"
string(66) "mb_str_pad(): Argument #3 ($pad_string) must be a non-empty string"
string(90) "mb_str_pad(): Argument #4 ($pad_type) must be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH"
string(82) "mb_str_pad(): Argument #5 ($encoding) must be a valid encoding, "unexisting" given"
--- Simple ASCII strings ---
string(7) "+Hello+"
string(10) "+-World+-+"
string(7) "+-Hello"
string(10) "+-+-+World"
string(7) "Hello+-"
string(10) "World+-+-+"
--- Edge cases pad length ---
string(6) "▶▶"
string(6) "▶▶"
string(6) "▶▶"
string(6) "▶▶"
--- No pad string ---
string(8) "▶▶  "
string(7) "▶▶ "
string(6) "▶▶"
string(6) "▶▶"
--- Empty input string ---
string(2) "  "
string(1) " "
string(0) ""
string(0) ""
--- No default argument ---
string(10) "▶▶    "
string(10) "    ▶▶"
string(10) "  ▶▶  "
--- UTF-8 emojis ---
string(18) "▶▶❤❓❇❤"
string(18) "❤❓❇❤▶▶"
string(18) "❤❓▶▶❤❓"
string(15) "▶▶❤❓❇"
string(15) "❤❓❇▶▶"
string(15) "❤▶▶❤❓"
string(12) "▶▶❤❓"
string(12) "❤❓▶▶"
string(12) "❤▶▶❤"
string(9) "▶▶❤"
string(9) "❤▶▶"
string(9) "▶▶❤"
string(6) "▶▶"
string(6) "▶▶"
string(6) "▶▶"
string(6) "▶▶"
string(6) "▶▶"
string(6) "▶▶"
--- UTF-8, 32, 7 test ---
string(92) "Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь▶▶▶"
string(92) "▶▶▶Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь"
string(92) "▶Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь▶▶"
string(92) "Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь▶▶▶"
string(92) "▶▶▶Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь"
string(92) "▶Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь▶▶"
string(92) "Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь▶▶▶"
string(92) "▶▶▶Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь"
string(92) "▶Σὲ γνωρίζω ἀπὸ τὴν κόψη Зарегистрируйтесь▶▶"
