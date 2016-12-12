--TEST--
bug#53872 (internal corruption of phar)
--SKIPIF--
<?php 
if (!extension_loaded("phar")) die("skip"); 
if (!extension_loaded("zlib")) die("skip Test needs ext/zlib"); 
?>
--INI--
phar.readonly=0
--FILE--
<?php
$p=new Phar('bug53872-phar.phar');
$p->buildFromDirectory(__DIR__ . "/bug53872/");
$p->setStub('<?php __HALT_COMPILER();?\>');
$p->compressFiles(Phar::GZ);

print(file_get_contents('phar://bug53872-phar.phar/first.txt'));
print(file_get_contents('phar://bug53872-phar.phar/second.txt'));
print(file_get_contents('phar://bug53872-phar.phar/third.txt'));
?>
--CLEAN--
<?php
unlink("bug53872-phar.phar");
?>
--EXPECT--
content of first.txt
content of third.txt

