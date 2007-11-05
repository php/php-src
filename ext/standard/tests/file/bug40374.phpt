--TEST--
Bug #40374 (php_shutdown_temporary_directory() tries to free local value) 
--FILE--
<?php

$file = tempnam(sys_get_temp_dir(), "test_");
var_dump($file);
$fp = fopen($file, "wt");
fwrite($fp, "test");
fclose($fp);
unlink($file);

echo "Done\n";
?>
--EXPECTF--	
string(%d) "%s"
Done
