--TEST--
Bug #41655 (open_basedir bypass via glob()) 1/2
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--INI--
open_basedir=/tmp
--FILE--
<?php
$a=glob("./*.jpeg");
var_dump($a);
echo "Done\n";
?>
--EXPECT--
bool(false)
Done