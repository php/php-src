--TEST--
Bug #67215 (php-cgi work with opcache, may be segmentation fault happen)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$file_c = __DIR__ . "/bug67215.c.php";
$file_p = __DIR__ . "/bug67215.p.php";
file_put_contents($file_c, "<?php require \"$file_p\"; class c extends p {} ?>");
file_put_contents($file_p, '<?php class p { protected $var = ""; } ?>');
require $file_c;
$a = new c();
require $file_c;
?>
--CLEAN--
<?php
$file_c = __DIR__ . "/bug67215.c.php";
$file_p = __DIR__ . "/bug67215.p.php";
unlink($file_c);
unlink($file_p);
?>
--EXPECTF--
Fatal error: Cannot declare class c, because the name is already in use in %sbug67215.c.php on line %d
