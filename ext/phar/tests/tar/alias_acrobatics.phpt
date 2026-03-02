--TEST--
Phar: alias edge cases
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.tar';

$p = new Phar($fname);

$p->setAlias('foo');
$p['unused'] = 'hi';
try {
$a = new Phar($fname2, 0, 'foo');
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
copy($fname, $fname2);
echo "2\n";
try {
$a = new Phar($fname2);
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
try {
$b = new Phar($fname, 0, 'another');
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.tar');
?>
--EXPECTF--
alias "foo" is already used for archive "%salias_acrobatics.phar.tar" cannot be overloaded with "%salias_acrobatics.2.phar.tar"
2
phar error: Unable to add tar-based phar "%salias_acrobatics.2.phar.tar", alias is already in use
alias "another" is already used for archive "%salias_acrobatics.phar.tar" cannot be overloaded with "%salias_acrobatics.phar.tar"
