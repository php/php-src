--TEST--
Transliterator - "id" property
--EXTENSIONS--
intl
--FILE--
<?php
$tr = Transliterator::create("Katakana-Latin");
echo $tr->id, "\n";
$revtr = $tr->createInverse();
echo $revtr->id, "\n";
var_dump($revtr);

echo "Done.\n";
?>
--EXPECTF--
Katakana-Latin
Latin-Katakana
object(Transliterator)#%d (%d) {
  ["id"]=>
  string(%d) "Latin-Katakana"
}
Done.
