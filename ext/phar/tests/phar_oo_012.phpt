--TEST--
Phar object: unset file
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$pharconfig = 0;

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass('SplFileObject');

$phar['f.php'] = 'hi';
var_dump(isset($phar['f.php']));
echo $phar['f.php'];
echo "\n";
unset($phar['f.php']);
var_dump(isset($phar['f.php']));

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_012.phar.php');
__halt_compiler();
?>
--EXPECT--
bool(true)
hi
bool(false)
