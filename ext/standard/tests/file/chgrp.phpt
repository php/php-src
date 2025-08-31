--TEST--
chgrp() with NULL as group name
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip, not supported on Windows");
?>
--FILE--
<?php
try {
    chgrp("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", 0);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECTF--
Warning: chgrp(): No such file or directory in %s on line %d
