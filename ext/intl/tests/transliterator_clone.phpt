--TEST--
Transliterator clone handler
--EXTENSIONS--
intl
--FILE--
<?php
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
