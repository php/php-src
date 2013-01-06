--TEST--
fclose() actually closes streams with refcount > 1
--FILE--
<?php
$s = fopen(__FILE__, "rb");
function separate_zval(&$var) { }
$s2 = $s;
separate_zval($s2);
fclose($s);
echo fread($s2, strlen("<?php"));
echo "\nDone.\n";
--EXPECTF--
Warning: fread(): %d is not a valid stream resource in %s on line %d

Done.
