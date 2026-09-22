--TEST--
GH-19038 (Phar crash and data corruption with SplFileObject)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

$phar = new Phar(__DIR__ . "/gh19038.phar");
$phar->addFromString("file", "123");
$file = $phar["file"]->openFile();
$file->fseek(3);
var_dump($file->fwrite("456", 3));
$file->fseek(0);
echo $file->fread(100);

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh19038.phar");
?>
--EXPECT--
int(3)
123456
