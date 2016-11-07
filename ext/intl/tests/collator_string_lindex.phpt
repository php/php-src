--TEST--
Collator::lindex()
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
	$coll->lindex($haystack1, $e_acute_nfc, 14) === 16,
	$coll->lindex($haystack2, $e_acute_nfd, 14) === 16,
	$coll->lindex($haystack1, $e_acute_nfc, -6) === 16,
	$coll->lindex($haystack2, $e_acute_nfd, -6) === 16
);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
