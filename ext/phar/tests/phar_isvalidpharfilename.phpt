--TEST--
Phar: Phar::isValidPharFilename()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.readonly=1
--FILE--
<?php
chdir(dirname(__FILE__));
Phar::isValidPharFilename(array());
echo "*\n";
var_dump(Phar::isValidPharFilename('*'));
var_dump(Phar::isValidPharFilename('*', true));
var_dump(Phar::isValidPharFilename('*', false));

echo "\nboo.phar\n";
var_dump(Phar::isValidPharFilename('boo.phar'));
var_dump(Phar::isValidPharFilename('boo.phar', true));
var_dump(Phar::isValidPharFilename('boo.phar', false));

echo "\nboo.tar\n";
var_dump(Phar::isValidPharFilename('boo.tar'));
var_dump(Phar::isValidPharFilename('boo.tar', true));
var_dump(Phar::isValidPharFilename('boo.tar', false));

echo "\nboo.phar.tar\n";
var_dump(Phar::isValidPharFilename('boo.phar.tar'));
var_dump(Phar::isValidPharFilename('boo.phar.tar', true));
var_dump(Phar::isValidPharFilename('boo.phar.tar', false));

mkdir(dirname(__FILE__) . '/.phar');

echo "\n.phar/boo.tar\n";
var_dump(Phar::isValidPharFilename('.phar/boo.tar'));
var_dump(Phar::isValidPharFilename('.phar/boo.tar', true));
var_dump(Phar::isValidPharFilename('.phar/boo.tar', false));

echo "\n.phar.tar\n";
var_dump(Phar::isValidPharFilename('.phar.tar'));
var_dump(Phar::isValidPharFilename('.phar.tar', true));
var_dump(Phar::isValidPharFilename('.phar.tar', false));

echo "\n.phar.phar\n";
var_dump(Phar::isValidPharFilename('.phar.phar'));
var_dump(Phar::isValidPharFilename('.phar.phar', true));
var_dump(Phar::isValidPharFilename('.phar.phar', false));

echo "\n.phar.phart\n";
var_dump(Phar::isValidPharFilename('.phar.phart'));
var_dump(Phar::isValidPharFilename('.phar.phart', true));
var_dump(Phar::isValidPharFilename('.phar.phart', false));

echo "\nmy.pharmy\n";
var_dump(Phar::isValidPharFilename('my.pharmy'));
var_dump(Phar::isValidPharFilename('my.pharmy', true));
var_dump(Phar::isValidPharFilename('my.pharmy', false));

echo "\nphar.zip\n";
var_dump(Phar::isValidPharFilename('phar.zip'));
var_dump(Phar::isValidPharFilename('phar.zip', true));
var_dump(Phar::isValidPharFilename('phar.zip', false));

echo "\nphar.zip.phar\n";
var_dump(Phar::isValidPharFilename('phar.zip.phar'));
var_dump(Phar::isValidPharFilename('phar.zip.phar', true));
var_dump(Phar::isValidPharFilename('phar.zip.phar', false));

echo "\ndir.phar.php\n";
var_dump(Phar::isValidPharFilename('dir.phar.php'));
var_dump(Phar::isValidPharFilename('dir.phar.php', true));
var_dump(Phar::isValidPharFilename('dir.phar.php', false));

?>
===DONE===
--CLEAN--
<?php
rmdir(dirname(__FILE__) . '/.phar');
--EXPECTF--
Warning: Phar::isValidPharFilename() expects parameter 1 to be %string, array given in %sphar_isvalidpharfilename.php on line %d
*
bool(false)
bool(false)
bool(false)

boo.phar
bool(true)
bool(true)
bool(false)

boo.tar
bool(false)
bool(false)
bool(true)

boo.phar.tar
bool(true)
bool(true)
bool(false)

.phar/boo.tar
bool(false)
bool(false)
bool(true)

.phar.tar
bool(false)
bool(false)
bool(true)

.phar.phar
bool(true)
bool(true)
bool(false)

.phar.phart
bool(false)
bool(false)
bool(true)

my.pharmy
bool(false)
bool(false)
bool(true)

phar.zip
bool(false)
bool(false)
bool(true)

phar.zip.phar
bool(true)
bool(true)
bool(false)

dir.phar.php
bool(true)
bool(true)
bool(false)
===DONE===

