--TEST--
Bug #41693 (scandir() allows empty directory names) 
--FILE--
<?php

var_dump(scandir(''));

echo "Done\n";
?>
--EXPECTF--	
Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)
Done
