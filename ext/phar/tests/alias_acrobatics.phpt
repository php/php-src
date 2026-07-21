--TEST--
Phar: alias edge cases
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar';

$p = new Phar($fname);

$p->setAlias('foo');
$p['unused'] = 'hi';
try {
$a = new Phar($fname2, 0, 'foo');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
copy($fname, $fname2);
echo "2\n";
try {
$a = new Phar($fname2);
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$b = new Phar($fname, 0, 'another');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar');
?>
--EXPECTF--
UnexpectedValueException: alias "foo" is already used for archive "%salias_acrobatics.phar" cannot be overloaded with "%salias_acrobatics.2.phar"
2
UnexpectedValueException: Cannot open archive "%salias_acrobatics.2.phar", alias is already in use by existing archive
UnexpectedValueException: alias "another" is already used for archive "%salias_acrobatics.phar" cannot be overloaded with "%salias_acrobatics.phar"
