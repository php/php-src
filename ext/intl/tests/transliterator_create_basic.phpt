--TEST--
Transliterator::create (basic)
--EXTENSIONS--
intl
--FILE--
<?php
$t = Transliterator::create("any-latin");
echo $t->id,"\n";

$t = transliterator_create("any-latin");
echo $t->id,"\n";

echo "Done.\n";
?>
--EXPECT--
any-latin
any-latin
Done.
