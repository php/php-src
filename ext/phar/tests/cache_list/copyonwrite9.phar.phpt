--TEST--
Phar: copy-on-write test 9 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite9.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write9.phar
--EXPECTF--
string(188) "<?php
$p = new Phar(__FILE__);
var_dump($p->getStub());
$p2 = new Phar(__FILE__);
$p->setStub("<?php __HALT"."_COMPILER();");
echo $p2->getStub(),"\n";
echo "ok\n";
__HALT_COMPILER(); ?>
"
<?php __HALT_COMPILER(); ?>

ok
