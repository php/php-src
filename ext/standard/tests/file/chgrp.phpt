--TEST--
chgrp() with NULL as group name
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip, not supported on Windows");
?>
--FILE--
<?php
chgrp("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", NULL);
echo "ALIVE\n";
?>
--EXPECTF--
Warning: chgrp(): parameter 2 should be string or integer, null given in %schgrp.php on line 2
ALIVE
