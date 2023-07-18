--TEST--
Phar: copy-on-write test 10 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite10.phar.php
phar.readonly=0
--EXTENSIONS--
phar
--FILE_EXTERNAL--
files/write10.phar
--EXPECTF--
string(214) "<?php
$p = new Phar(__FILE__);
var_dump($p->getStub());
$p2 = new Phar(__FILE__);
$a = fopen("phar://" . __FILE__ . "/test.txt", "r");
$p->setStub($a);
echo $p2->getStub(),"\n";
echo "ok\n";
__HALT_COMPILER(); ?>
"

Deprecated: Calling Phar::setStub(resource $stub, int $length) is deprecated in %s on line %d
<?php __HALT_COMPILER(); ?>

ok
