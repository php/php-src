--TEST--
GH-25722 (Double free of a phar archive shared by objects outliving request shutdown)
--EXTENSIONS--
phar
--CREDITS--
sgInnora
--FILE--
<?php
$fname = __DIR__ . '/gh25722.tar';

$p = new PharData($fname);
$a = &$p;
$q = new PharData($fname);
$b = &$q;

echo "Done", PHP_EOL;
?>
--EXPECT--
Done
