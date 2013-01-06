--TEST--
Transliterator::create (basic)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$t = Transliterator::create("any-latin");
echo $t->id,"\n";

$t = transliterator_create("any-latin");
echo $t->id,"\n";

echo "Done.\n";

--EXPECT--
any-latin
any-latin
Done.

