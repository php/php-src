--TEST--
chown() with NULL as user name
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip, not supported on Windows");
?>
--FILE--
<?php
chown("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", NULL);
echo "ALIVE\n";
?>
--EXPECTF--
Warning: chown(): parameter 2 should be string or int, null given in %schown.php on line %d
ALIVE
