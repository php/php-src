--TEST--
chown() with NULL as user name
--FILE--
<?php
chown("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", NULL);
echo "ALIVE\n";
?>
--EXPECTF--
Warning: chown(): parameter 2 should be string or integer, null given in %schown.php on line %d
ALIVE
