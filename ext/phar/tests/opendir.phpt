--TEST--
Phar: test opendir() interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.require_hash=1
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$a = new Phar($fname);
$a['index.php'] = '<?php
$a = opendir("dir");
while (false !== ($e = readdir($a))) {
    echo $e;
}
?>';
$a['dir/file1.txt'] = 'hi';
$a['dir/file2.txt'] = 'hi2';
$a['dir/file3.txt'] = 'hi3';
$a->setStub('<?php
Phar::interceptFileFuncs();
set_include_path("phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
include $fname;
echo "\n";
opendir('phar://');
opendir('phar://hi.phar');
ini_set('phar.extract_list', 'hi.phar='.dirname(__FILE__));
opendir('phar://hi.phar/oopsie/daisy/');
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
file1.txtfile2.txtfile3.txt

Warning: opendir(phar://): failed to open dir: phar error: no directory in "phar://", must have at least phar:/// for root directory (always use full path to a new phar)
phar url "phar://" is unknown in %sopendir.php on line %d

Warning: opendir(phar://hi.phar): failed to open dir: phar url "phar://hi.phar" is unknown in %sopendir.php on line %d

Warning: opendir(phar://hi.phar/oopsie/daisy/): failed to open dir: phar error: file "%soopsie/daisy" extracted from "hi.phar" could not be opened in %sopendir.php on line %d
===DONE===