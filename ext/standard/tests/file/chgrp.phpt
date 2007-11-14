--TEST--
chgrp() with NULL as group name
--FILE--
<?php
chgrp("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", NULL);
echo "ALIVE\n";
?>
--EXPECT--
ALIVE
