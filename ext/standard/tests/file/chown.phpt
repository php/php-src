--TEST--
chown() with NULL as user name
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip, not supported on Windows");
?>
--FILE--
<?php
try {
    chown("sjhgfskhagkfdgskjfhgskfsdgfkdsajf", NULL);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "ALIVE\n";
?>
--EXPECTF--
Warning: chown(): No such file or directory in %s on line %d
ALIVE
