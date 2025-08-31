--TEST--
Phar: test file_get_contents() interception
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
echo file_get_contents("dir/file1.txt");
echo file_get_contents("file1.txt", true);
?>';
$a['dir/file1.txt'] = 'hi';
$a['dir/file2.txt'] = 'hi2';
$a['dir/file3.txt'] = 'hi3';
$a->setStub('<?php
Phar::interceptFileFuncs();
set_include_path("phar://" . __FILE__ . "/dir" . PATH_SEPARATOR . "phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
include $fname;
?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
hihi===DONE===
