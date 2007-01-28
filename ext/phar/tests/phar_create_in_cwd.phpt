--TEST--
Phar: create a Phar with relative path
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
chdir(dirname(__FILE__));
$p = new Phar('brandnewphar.phar');
$p['file1.txt'] = 'hi';
$p->commit();
var_dump($p->getStub());
$p->setStub("<?php
function __autoload(\$class)
{
    include 'phar://' . str_replace('_', '/', \$class);
}
Phar::mapPhar('brandnewphar.phar');
include 'phar://brandnewphar.phar/startup.php';
__HALT_COMPILER();
?>");
var_dump($p->getStub());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/brandnewphar.phar');
?>
--EXPECT--
string(24) "<?php __HALT_COMPILER();"
string(198) "<?php
function __autoload($class)
{
    include 'phar://' . str_replace('_', '/', $class);
}
Phar::mapPhar('brandnewphar.phar');
include 'phar://brandnewphar.phar/startup.php';
__HALT_COMPILER();
?>"
===DONE===