--TEST--
test broken app
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$file = "zfapp";
$tgz_file = dirname(__FILE__) . "/files/$file.tgz";
chdir(dirname(__FILE__));

$phar_file = basename(__FILE__, '.php') . '.phar';
@unlink($phar_file);
copy($tgz_file, $phar_file);

$phar = new Phar($phar_file);
$phar->startBuffering();
$phar->setStub("<?php
Phar::interceptFileFuncs();
Phar::webPhar('$file.phar', 'html/index.php');
echo 'BlogApp is intended to be executed from a web browser\n';
exit -1;
__HALT_COMPILER();
");
$phar->stopBuffering();

foreach(new RecursiveIteratorIterator($phar) as $path) {
    echo str_replace('\\', '/', $path->getPathName()) . "\n";
}

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
__HALT_COMPILER();
?>
--EXPECTF--
phar://%szf_test.phar/application/default/controllers/ErrorController.php
phar://%szf_test.phar/application/default/controllers/IndexController.php
phar://%szf_test.phar/application/default/views/scripts/error/error.phtml
phar://%szf_test.phar/application/default/views/scripts/index/index.phtml
phar://%szf_test.phar/html/.htaccess
phar://%szf_test.phar/html/index.php
===DONE===
