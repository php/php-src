--TEST--
Bug #65559 (cache not cleared if changes occur while running)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=2
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$file =  __DIR__ . "/bug6559.inc.php";
file_put_contents($file, '<?php return 1;');
$var = include $file;
var_dump($var);
file_put_contents($file, '<?php return 2;');
$var = include $file;
var_dump($var);
@unlink($file);
?>
--EXPECT--
int(1)
int(2)
