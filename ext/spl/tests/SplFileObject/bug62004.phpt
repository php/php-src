--TEST--
Bug #62004 (SplFileObject: fgets after seek returns wrong line)
--FILE--
<?php
$f = new SplFileObject(__DIR__ . '/bug62004.txt');
$f->setFlags(SplFileObject::SKIP_EMPTY);
$f->seek(0);
echo $f->fgets();
$f->seek(1);
echo $f->fgets();
$f->seek(2);
echo $f->fgets();
?>
--EXPECT--
Line 1
Line 2
Line 3
