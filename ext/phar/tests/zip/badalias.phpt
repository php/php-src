--TEST--
Phar: invalid aliases
--INI--
phar.require_hash=0
--EXTENSIONS--
phar
zlib
bz2
--FILE--
<?php
$e = __DIR__ . '/files/';
for ($i = 1; $i <= 5; $i++) {
try {
new Phar($e . "badalias$i.phar.zip");
} catch (Exception $ee) {
echo $ee::class, ': ', $ee->getMessage(), "\n";
}
}
?>
--EXPECTF--
UnexpectedValueException: phar error: invalid alias "hi/there" in zip-based phar "%sbadalias1.phar.zip"
UnexpectedValueException: phar error: invalid alias "hi\there" in zip-based phar "%sbadalias2.phar.zip"
UnexpectedValueException: phar error: invalid alias "hi\there" in zip-based phar "%sbadalias3.phar.zip"
UnexpectedValueException: phar error: invalid alias "hi;there" in zip-based phar "%sbadalias4.phar.zip"
UnexpectedValueException: phar error: invalid alias "hi:there" in zip-based phar "%sbadalias5.phar.zip"
