--TEST--
Phar: PHP bug #74991: include_path has a 4096 char (minus "__DIR__:") limit, in some PHAR cases
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
// create a sample file in a custom include_path to lookup from the phar later:
mkdir('path');
touch('path/needle.php');
$p = new Phar('sample.phar');
// the use of a sub path is crucial, and make the include_path 1 byte larger (=OVERFLOW) than the MAXPATHLEN, the include_path will then be truncated to 4096 (MAXPATHLEN) into 'phar://..sample.phar/some:xx..xx:pat' so it will fail to find needle.php:
$p['some/file'] = "<?php const MAXPATHLEN = 4096, OVERFLOW = 1, PATH = 'path'; set_include_path(str_repeat('x', MAXPATHLEN - strlen(__DIR__ . PATH_SEPARATOR . PATH_SEPARATOR . PATH) + OVERFLOW) . PATH_SEPARATOR . PATH); require('needle.php');";
$p->setStub("<?php Phar::mapPhar('sample.phar'); __HALT_COMPILER();");
// execute the phar code:
require('phar://sample.phar/some/file');
?>
--CLEAN--
<?php
unlink('path/needle.php');
unlink('sample.phar');
rmdir('path');
?>
--EXPECT--
