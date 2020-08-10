--TEST--
Bug #46053 (SplFileObject::seek - Endless loop)
--FILE--
<?php

$x = new splfileobject(__FILE__);
$x->getPathName();
$x->seek(10);
$x->seek(0);
var_dump(trim($x->fgets()));
?>
--EXPECTF--
string(%d) "<?php"
