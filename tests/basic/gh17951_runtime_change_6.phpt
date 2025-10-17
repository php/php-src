--TEST--
GH-17951 Runtime Change 6
--INI--
memory_limit=128M
max_memory_limit=512M
--FILE--
<?php
for($i = 0; $i < 3; $i++) {
  ini_set('memory_limit', '1024M');
  echo ini_get('memory_limit');
}
?>
--EXPECTF--
Warning: Failed to set memory_limit to 1073741824 bytes. Setting to max_memory_limit instead (currently: 536870912 bytes) in %s on line %d
512M
Warning: Failed to set memory_limit to 1073741824 bytes. Setting to max_memory_limit instead (currently: 536870912 bytes) in %s on line %d
512M
Warning: Failed to set memory_limit to 1073741824 bytes. Setting to max_memory_limit instead (currently: 536870912 bytes) in %s on line %d
512M
