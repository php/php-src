--TEST--
Fix GH-13984: Buffer size is now checked before memcmp
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$dbfile = 'z.db';
$db = new PDO('sqlite:' . $dbfile, null, null, [PDO::ATTR_PERSISTENT => true]);
echo 'done!';
?>
--CLEAN--
<?php
@unlink(getcwd() . '/z.db');
?>
--EXPECT--
done!
