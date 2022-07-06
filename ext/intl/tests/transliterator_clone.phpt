--TEST--
Transliterator clone handler
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$str = "a U+4E07";

$t = Transliterator::create("hex-any");
echo $t->id, ": ", $t->transliterate($str), "\n";

$u = clone $t;
echo $u->id, ": ", $u->transliterate($str), "\n";

echo "Done.\n";
?>
--EXPECT--
hex-any: a 万
hex-any: a 万
Done.
