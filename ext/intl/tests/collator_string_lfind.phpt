--TEST--
Collator::lfind()
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

var_dump(
	$coll->lfind($haystack1, $e_acute_nfc) === "{$e_acute_nfd}br{$e_acute_nfd}ch{$e_acute_nfd}.",
	$coll->lfind($haystack2, $e_acute_nfd) === "{$e_acute_nfc}br{$e_acute_nfc}ch{$e_acute_nfc}.",

	$coll->lfind($haystack1, $e_acute_nfc, 0, TRUE) === "Ce verre est ",
	$coll->lfind($haystack2, $e_acute_nfd, 0, TRUE) === "Ce verre est ",

	$coll->lfind($haystack1, $e_acute_nfc, 14) === "{$e_acute_nfd}ch{$e_acute_nfd}.",
	$coll->lfind($haystack2, $e_acute_nfd, 14) === "{$e_acute_nfc}ch{$e_acute_nfc}."
);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
