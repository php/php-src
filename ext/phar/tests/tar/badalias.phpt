--TEST--
Phar: invalid aliases
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip no zlib"); ?>
<?php if (!extension_loaded("bz2")) die("skip no bz2"); ?>
--FILE--
<?php
$e = dirname(__FILE__) . '/files/';
for ($i = 1; $i <= 5; $i++) {
try {
new Phar($e . "badalias$i.phar.tar");
} catch (Exception $ee) {
echo $ee->getMessage(), "\n";
}
}
?>
===DONE===
--EXPECTF--
phar error: invalid alias "hi/thereaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..." in tar-based phar "%sbadalias1.phar.tar"
phar error: invalid alias "hi\there" in tar-based phar "%sbadalias2.phar.tar"
phar error: invalid alias "hi;there" in tar-based phar "%sbadalias3.phar.tar"
phar error: invalid alias "hi:there" in tar-based phar "%sbadalias4.phar.tar"
phar error: tar-based phar "%sbadalias5.phar.tar" has alias that is larger than 511 bytes, cannot process
===DONE===
