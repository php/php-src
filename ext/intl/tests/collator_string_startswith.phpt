--TEST--
Collator::startswith()
--SKIPIF--
<?php if (!extension_loaded('intl')) echo 'skip'; ?>
--INI--
intl.error_level=0
intl.use_exceptions=0
--FILE--
<?php
$grave = "\u{300}";
$acute = "\u{301}";

$e_acute_nfc = "é"; // U+E9
$e_acute_nfd = "e${acute}";

$e_grave_nfc = "è"; // U+E8
$e_grave_nfd = "e${grave}";

$fr = new Collator('fr');
$tr = new Collator('tr');
$fr->setStrength(Collator::PRIMARY);

var_dump($fr->startswith('SSSSe', 'ßß'));
var_dump($fr->startswith('ßße', 'SSSS'));

foreach ([$fr, $tr] as $coll) {
	$coll->setStrength(Collator::SECONDARY);
}

echo "\n", 'Grapheme consistency', "\n";
var_dump($fr->startswith($e_acute_nfd, 'e'));

echo "\n", 'Turkish dotted I', "\n";
foreach ([$fr, $tr] as $coll) {
	echo $coll->getLocale(Locale::VALID_LOCALE), PHP_EOL;
	var_dump($coll->startswith('İyi', 'i'));
	var_dump($coll->startswith('Iyi', 'i'));
}

echo "\n", 'Turkish non dotted I', "\n";
foreach ([$fr, $tr] as $coll) {
	echo $coll->getLocale(Locale::VALID_LOCALE), PHP_EOL;
	var_dump($coll->startswith('Hayır', 'HAYI'));
	var_dump($coll->startswith('Hayir', 'HAYI'));
}
?>
--EXPECTF--
bool(true)
bool(true)

Grapheme consistency
bool(false)

Turkish dotted I
fr
bool(false)
bool(true)
tr
bool(true)
bool(false)

Turkish non dotted I
fr
bool(false)
bool(true)
tr
bool(true)
bool(false)
