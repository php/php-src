--TEST--
Collator::replaceCallback
--SKIPIF--
<?php if (!extension_loaded('intl')) echo 'skip'; ?>
--FILE--
<?php
const STRING = 'élève';

$coll = new Collator(NULL);
$coll->setStrength(Collator::PRIMARY);

var_dump(
	$coll->replaceCallback(STRING, 'e', 'htmlentities'),
	$coll->replaceCallback(STRING, 'e', function () {}),
	$coll->replaceCallback(STRING, 'e', function () { return 3; }),
	$coll->replaceCallback(STRING, 'e', function ($m) { return '<mark>' . $m . '</mark>'; })
);
try {
	$coll->replaceCallback(STRING, 'e', function ($m, $n) {});
} catch (Error $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
string(19) "&eacute;l&egrave;ve"
string(2) "lv"
string(5) "3l3v3"
string(46) "<mark>é</mark>l<mark>è</mark>v<mark>e</mark>"
Too few arguments to function {closure}(), 1 passed and exactly 2 expected
