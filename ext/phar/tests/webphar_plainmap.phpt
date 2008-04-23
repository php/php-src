--TEST--
Phar: Phar::webPhar with phar.extract_list
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.readonly=1
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.plain.php';

file_put_contents($fname, '<?php Phar::webPhar("myphar.phar", "index.php");');
ini_set('phar.extract_list', 'myphar.phar=' . dirname(__FILE__));

try {
include 'phar://myphar.phar/' . basename($fname);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.plain.php');
?>
--EXPECT--
Cannot use Phar::webPhar() from an extracted phar archive, simply use the extracted files directly