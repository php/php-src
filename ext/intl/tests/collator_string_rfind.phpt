--TEST--
Collator::rfind()
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

echo 'Same string in NFC and NFD forms:', "\n";
var_dump(
	$coll->rfind($haystack1, $e_acute_nfc) === "{$e_acute_nfd}.",
	$coll->rfind($haystack2, $e_acute_nfd) === "{$e_acute_nfc}.",

	$coll->rfind($haystack1, $e_acute_nfc, 0, TRUE) === "Ce verre est {$e_acute_nfd}br{$e_acute_nfd}ch",
	$coll->rfind($haystack2, $e_acute_nfd, 0, TRUE) === "Ce verre est {$e_acute_nfc}br{$e_acute_nfc}ch",

	$coll->rfind($haystack1, $e_acute_nfc, 14) === "{$e_acute_nfd}br{$e_acute_nfd}ch{$e_acute_nfd}.",
	$coll->rfind($haystack2, $e_acute_nfd, 14) === "{$e_acute_nfc}br{$e_acute_nfc}ch{$e_acute_nfc}."
);

/*
+---+---+---+---+---+
| a | a | a | a | a |
+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 |
+---+---+---+---+---+
              # ^     => search on left to offset 4 (represented by ^), match is at offset 3 (represented by #)
  # ^                 => search on left to offset 1 (represented by ^), match is at offset 0 (represented by #)
*/

echo 'Simple offset checks:', "\n";
var_dump(
	$coll->rfind('aaaaa', 'a', 1, FALSE) === 'aaaaa',
	$coll->rfind('aaaaa', 'a', 1, TRUE) === '',

	$coll->rfind('aaaaa', 'a', 4, FALSE) === 'aa',
	$coll->rfind('aaaaa', 'a', 4, TRUE) === 'aaa'
);

$X307 = "\u{307}"; # 307 (COMBINING DOT ABOVE)
$X323 = "\u{323}"; # 323 (COMBINING DOT BELOW)
$S = "S{$X307}{$X323}";
$string = str_repeat($S, 5);
echo 'String of repeted substrings:', "\n";
var_dump(
	$coll->rfind($string, $S, 1, FALSE) === str_repeat($S, 5),
	$coll->rfind($string, $S, 1, TRUE) === '',

	$coll->rfind($string, $S, 4, FALSE) === str_repeat($S, 2),
	$coll->rfind($string, $S, 4, TRUE) === str_repeat($S, 3)
);

/*
var_dump($coll->rindex($string, $S, 4));
var_dump($coll->rindex($string, $S, 4));
var_dump($coll->rfind($string, $S, 4, FALSE));
var_dump($coll->rfind($string, $S, 4, TRUE));
var_dump($coll->rfind('aaaaa', 'a', 1, FALSE)); // string (5) "aaaaa" match on [0;1[
var_dump($coll->rfind('aaaaa', 'a', 1, TRUE)); // string (0) "" match on [0;1[
var_dump($coll->rfind('aaaaa', 'a', 4, FALSE)); // string (2) "aa" match on [3;4[
var_dump($coll->rfind('aaaaa', 'a', 4, TRUE)); // string (3) "aaa" match on [3;4[
*/
?>
--EXPECT--
Same string in NFC and NFD forms:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Simple offset checks:
bool(true)
bool(true)
bool(true)
bool(true)
String of repeted substrings:
bool(true)
bool(true)
bool(true)
bool(true)
