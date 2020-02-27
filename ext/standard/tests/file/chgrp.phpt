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
    chgrp("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", null);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
chgrp(): Argument #2 ($group) must be of type string|int, null given
