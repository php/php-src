--TEST--
Phar: phpinfo display 2
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (!extension_loaded("zlib")) die("skip zlib not loaded");
if (!extension_loaded("bz2")) die("skip bz2 not loaded");
$arr = Phar::getSupportedSignatures();
if (in_array("OpenSSL", $arr)) die("skip openssl support enabled");
?>
--INI--
phar.readonly=1
phar.require_hash=1
phar.cache_list=
--FILE--
<?php
phpinfo(INFO_MODULES);
?>
===DONE===
--EXPECTF--
%a
Phar

Phar: PHP Archive support => enabled
Phar EXT version => %s
Phar API version => 1.1.1
CVS revision => %sRevision: %s $
Phar-based phar archives => enabled
Tar-based phar archives => enabled
ZIP-based phar archives => enabled
gzip compression => enabled
bzip2 compression => enabled
OpenSSL support => disabled (install ext/openssl)


Phar based on pear/PHP_Archive, original concept by Davey Shafik.
Phar fully realized by Gregory Beaver and Marcus Boerger.
Portions of tar implementation Copyright (c) %d-%d Tim Kientzle.
Directive => Local Value => Master Value
phar.cache_list => no value => no value
phar.readonly => On => On
phar.require_hash => On => On
%a
===DONE===
