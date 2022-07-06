--TEST--
Phar::setStub()/stopBuffering()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$p = new Phar(__DIR__ . '/phar_commitwrite.phar', 0, 'phar_commitwrite.phar');
$p['file1.txt'] = 'hi';
$p->stopBuffering();
var_dump(strlen($p->getStub()));
$p->setStub("<?php
spl_autoload_register(function(\$class) {
    include 'phar://' . str_replace('_', '/', \$class);
});
Phar::mapPhar('phar_commitwrite.phar');
include 'phar://phar_commitwrite.phar/startup.php';
__HALT_COMPILER();
?>");
var_dump($p->getStub());
?>
--CLEAN--
<?php
unlink(__DIR__ . '/phar_commitwrite.phar');
__HALT_COMPILER();
?>
--EXPECTF--
int(6641)
string(%d) "<?php
spl_autoload_register(function($class) {
    include 'phar://' . str_replace('_', '/', $class);
});
Phar::mapPhar('phar_commitwrite.phar');
include 'phar://phar_commitwrite.phar/startup.php';
__HALT_COMPILER(); ?>
"
