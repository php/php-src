--TEST--
Phar::setStub()/stopBuffering() zip-based
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$p = new Phar(__DIR__ . '/phar_commitwrite.phar.zip', 0, 'phar_commitwrite.phar');
$p['file1.txt'] = 'hi';
$p->stopBuffering();
var_dump($p->getStub());
$p->setStub("<?php
spl_autoload_register(function(\$class) {
    include 'phar://' . str_replace('_', '/', \$class);
});
Phar::mapPhar('phar_commitwrite.phar');
include 'phar://phar_commitwrite.phar/startup.php';
__HALT_COMPILER();
?>");
var_dump($p->getStub());
var_dump($p->isFileFormat(Phar::ZIP));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/phar_commitwrite.phar.zip');
?>
--EXPECTF--
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
string(%d) "<?php
spl_autoload_register(function($class) {
    include 'phar://' . str_replace('_', '/', $class);
});
Phar::mapPhar('phar_commitwrite.phar');
include 'phar://phar_commitwrite.phar/startup.php';
__HALT_COMPILER(); ?>
"
bool(true)
