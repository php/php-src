--TEST--
Phar: invalid aliases
--EXTENSIONS--
phar
zlib
bz2
--FILE--
<?php
$e = __DIR__ . '/files/';
for ($i = 1; $i <= 5; $i++) {
try {
new Phar($e . "badalias$i.phar.tar");
} catch (Exception $ee) {
echo $ee->getMessage(), "\n";
}
}
?>
--EXPECTF--
phar error: invalid alias "hi/thereaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..." in tar-based phar "%sbadalias1.phar.tar"
phar error: invalid alias "hi\there" in tar-based phar "%sbadalias2.phar.tar"
phar error: invalid alias "hi;there" in tar-based phar "%sbadalias3.phar.tar"
phar error: invalid alias "hi:there" in tar-based phar "%sbadalias4.phar.tar"
phar error: tar-based phar "%sbadalias5.phar.tar" has alias that is larger than 511 bytes, cannot process
