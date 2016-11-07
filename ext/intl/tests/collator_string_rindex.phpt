--TEST--
Collator::rindex()
--SKIPIF--
<?php if (!extension_loaded('intl')) echo 'skip'; ?>
--INI--
intl.use_exceptions=0
--FILE--
<?php
$grave = "\u{300}";
$acute = "\u{301}";

$e_acute_nfc = "é";
$e_acute_nfd = "e${acute}";

$e_grave_nfc = "è";
$e_grave_nfd = "e${grave}";

$coll = new Collator('fr_FR');
$coll->setAttribute(Collator::NORMALIZATION_MODE, Collator::OFF);

$haystack1 = "Ce verre est {$e_acute_nfd}br{$e_acute_nfd}ch{$e_acute_nfd}.";
$haystack2 = str_replace($e_acute_nfd, $e_acute_nfc, $haystack1);

echo 'Same string in NFC and NFD forms (positive offset):', "\n";
var_dump(
	$coll->rindex($haystack1, $e_acute_nfc, 14) === 13,
	$coll->rindex($haystack2, $e_acute_nfd, 14) === 13
);

/*

NFD ($haystack1):
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| C | e | - | v | e | r | r | e | - | e | s | t | - | e | ' | b | r | ...
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | 1 | 2 |   3   | 4 | 5 | Grapheme offsets
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | CU offsets
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
                                                            ^ (search offset)

NFC ($haystack2):
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| C | e | - | v | e | r | r | e | - | e | s | t | - | é | b | r | é | c | h | é | . | ...
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | Grapheme offsets
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 1 | 0 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | Negative grapheme offsets (without minus sign)
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | CU offsets
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
                                                        ^ (search offset)
*/

echo 'Same string in NFC and NFD forms (negative offset):', "\n";
var_dump(
	$coll->rindex($haystack1, $e_acute_nfc, -7) === 13,
	$coll->rindex($haystack2, $e_acute_nfd, -7) === 13
);

echo 'Grapheme consistency:', "\n";
var_dump($coll->rindex($haystack1, 'e') === 9);

echo 'Overlap test (r[find|index] only):', "\n";
var_dump(
	$coll->rindex($haystack1, $e_acute_nfc, 13) === -1,
	$coll->rindex($haystack2, $e_acute_nfd, 13) === -1
);

exit;
?>
--EXPECT--
Same string in NFC and NFD forms (positive offset):
bool(true)
bool(true)
Same string in NFC and NFD forms (negative offset):
bool(true)
bool(true)
Grapheme consistency:
bool(true)
Overlap test (r[find|index] only):
bool(true)
bool(true)
