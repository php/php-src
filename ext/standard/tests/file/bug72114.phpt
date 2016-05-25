--TEST--
Bug #72114 (Integer underflow / arbitrary null write in fread/gzread)
--FILE--
<?php
ini_set('memory_limit', "2500M");
$fp = fopen("/dev/zero", "r");
fread($fp, 2147483648);
?>
Done
--EXPECTF--
Warning: fread(): Length parameter must be no more than 2147483647 in %s/bug72114.php on line %d
Done
