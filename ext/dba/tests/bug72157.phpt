--TEST--
Bug #72157 (use-after-free caused by dba_open)
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php
$var0 = fopen(__FILE__,"r");
$var5 = dba_open(null,$var0);
$var5 = dba_open(null,$var0);
$var5 = dba_open(null,$var0);
$var5 = dba_open($var0,$var0);
?>
--EXPECTF--
Warning: dba_open(,Resource id #5): Illegal DBA mode in %sbug72157.php on line %d

Warning: dba_open(,Resource id #5): Illegal DBA mode in %sbug72157.php on line %d

Warning: dba_open(,Resource id #5): Illegal DBA mode in %sbug72157.php on line %d

Warning: dba_open(Resource id #5,Resource id #5): Illegal DBA mode in %sbug72157.php on line %d
