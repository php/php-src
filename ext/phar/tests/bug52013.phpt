--TEST--
Test for bug 52013 about Phar::decompressFiles().
--DESCRIPTION--
Test for a bug where Phar::decompressFiles() mistakenly throws BadMethodCallException.
http://bugs.php.net/bug.php?id=52013
--CREDITS--
Frederic Hardy frederic.hardy@mageekbox.net
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
open_basedir=
--FILE--
<?php
mkdir(dirname(__FILE__) . '/testdir');
file_put_contents(dirname(__FILE__) . '/testdir/1.php', str_repeat(' ', 1455));

$phar = new Phar(dirname(__FILE__) . '/compressed.phar');
$phar->buildFromDirectory(dirname(__FILE__) . '/testdir', '/\.php$/');
$phar->setSignatureAlgorithm(Phar::SHA1);
$phar->compressFiles(Phar::GZ);
$phar->decompressFiles();

echo 'ok';
?>
--CLEAN--
<?php
if (is_file(dirname(__FILE__) . '/testdir/1.php'))
  unlink(dirname(__FILE__) . '/testdir/1.php');
if (is_dir(dirname(__FILE__) . '/testdir'))
  rmdir(dirname(__FILE__) . '/testdir');
if (is_file(dirname(__FILE__) . '/compressed.phar'))
  unlink(dirname(__FILE__) . '/compressed.phar');
?>
--EXPECT--
ok
