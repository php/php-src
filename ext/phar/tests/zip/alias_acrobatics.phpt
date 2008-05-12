--TEST--
Phar: alias edge cases
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.phar.zip';

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
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.zip');
?>
--EXPECTF--
alias "foo" is already used for archive "%salias_acrobatics.phar.zip" cannot be overloaded with "%salias_acrobatics.2.phar.zip"
2
phar error: Unable to add zip-based phar "%salias_acrobatics.2.phar.zip" with implicit alias, alias is already in use
alias "another" is already used for archive "%salias_acrobatics.phar.zip" cannot be overloaded with "%salias_acrobatics.phar.zip"
===DONE===
