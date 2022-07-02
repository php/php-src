--TEST--
Phar: test broken app
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$file = "zfapp";
$orig_file = __DIR__ . "/files/$file.tgz";
$tgz_file = __DIR__ . "/$file.tgz";
$phar_file = __DIR__ . "/$file.phar.tar.gz";
copy($orig_file, $tgz_file);

$phar = new PharData($tgz_file);
$phar = $phar->convertToExecutable();

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
--CLEAN--
<?php
unlink(__DIR__ . '/zfapp.tgz');
unlink(__DIR__ . '/zfapp.phar.tar.gz');
?>
--EXPECTF--
phar://%szfapp.phar.tar.gz/application/default/controllers/ErrorController.php
phar://%szfapp.phar.tar.gz/application/default/controllers/IndexController.php
phar://%szfapp.phar.tar.gz/application/default/views/scripts/error/error.phtml
phar://%szfapp.phar.tar.gz/application/default/views/scripts/index/index.phtml
phar://%szfapp.phar.tar.gz/html/.htaccess
phar://%szfapp.phar.tar.gz/html/index.php
