--TEST--
Bug #46053 (SplFileObject::seek - Endless loop)
--FILE--
<?php

$x = new SplFileObject(__FILE__);
$x->getPathname();
$x->seek(10);
$x->seek(0);
var_dump(trim($x->fgets()));
?>
--EXPECTF--
string(%d) "<?php"
