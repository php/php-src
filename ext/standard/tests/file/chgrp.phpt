--TEST--
chgrp() with NULL as group name
--FILE--
<?php
chgrp("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", NULL);
echo "ALIVE\n";
?>
--EXPECTF--
Warning: chgrp(): parameter 2 should be string or integer, null given in %schgrp.php on line 2
ALIVE
