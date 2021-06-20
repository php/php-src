--TEST--
Test for bug 52013 about Phar::decompressFiles().
--DESCRIPTION--
Test for a bug where Phar::decompressFiles() mistakenly throws BadMethodCallException.
http://bugs.php.net/bug.php?id=52013
--CREDITS--
Frederic Hardy frederic.hardy@mageekbox.net
--EXTENSIONS--
phar
zlib
--INI--
phar.require_hash=0
phar.readonly=0
open_basedir=
--FILE--
<?php
mkdir(__DIR__ . '/testdir');
file_put_contents(__DIR__ . '/testdir/1.php', str_repeat(' ', 1455));

$phar = new Phar(__DIR__ . '/compressed.phar');
$phar->buildFromDirectory(__DIR__ . '/testdir', '/\.php$/');
$phar->setSignatureAlgorithm(Phar::SHA1);
$phar->compressFiles(Phar::GZ);
$phar->decompressFiles();

echo 'ok';
?>
--CLEAN--
<?php
if (is_file(__DIR__ . '/testdir/1.php'))
  unlink(__DIR__ . '/testdir/1.php');
if (is_dir(__DIR__ . '/testdir'))
  rmdir(__DIR__ . '/testdir');
if (is_file(__DIR__ . '/compressed.phar'))
  unlink(__DIR__ . '/compressed.phar');
?>
--EXPECT--
ok
