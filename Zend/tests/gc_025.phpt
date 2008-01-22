--TEST--
GC 025: Automatic GC on request shutdown
--FILE--
<?php
$a = array(array());
$a[0][0] =& $a[0];
unset($a);
echo "ok\n"
?>
--EXPECT--
ok
