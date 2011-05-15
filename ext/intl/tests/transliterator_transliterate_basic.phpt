--TEST--
Transliterator::transliterate (basic)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$t = transliterator_create("Latin; Title");
$s = "Κοντογιαννάτος, Βασίλης";
echo $t->transliterate($s),"\n";
echo transliterator_transliterate($t, $s),"\n";
echo $t->transliterate($s, 3),"\n";
echo $t->transliterate($s, 3, 4),"\n";

echo "Done.\n";
--EXPECT--
Kontogiannátos, Basílēs
Kontogiannátos, Basílēs
ΚονTogiannátos, Basílēs
ΚονTογιαννάτος, Βασίλης
Done.
