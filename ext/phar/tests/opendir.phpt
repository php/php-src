--TEST--
Phar: test opendir() interception
--EXTENSIONS--
phar
--INI--
phar.require_hash=1
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$a = new Phar($fname);
$a['index.php'] = '<?php
$a = opendir("dir");
if ($a) {
    while (false !== ($e = readdir($a))) {
        echo $e;
    }
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
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
file1.txtfile2.txtfile3.txt

Warning: opendir(phar://): Failed to open directory: phar error: no directory in "phar://", must have at least phar:/// for root directory (always use full path to a new phar)
phar url "phar://" is unknown in %sopendir.php on line %d

Warning: opendir(phar://hi.phar): Failed to open directory: phar error: invalid url or non-existent phar "phar://hi.phar"
phar url "phar://hi.phar" is unknown in %sopendir.php on line %d
