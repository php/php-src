--TEST--
Phar: phpinfo display 2
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not loaded"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not loaded"); ?>
--INI--
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
CVS revision => $Revision$
Phar-based phar archives => enabled
Tar-based phar archives => enabled
ZIP-based phar archives => enabled
gzip compression => enabled
bzip2 compression => enabled


Phar based on pear/PHP_Archive, original concept by Davey Shafik.
Phar fully realized by Gregory Beaver and Marcus Boerger.
Portions of tar implementation Copyright (c) 2003-2007 Tim Kientzle.
Directive => Local Value => Master Value
phar.extract_list =>  => 
phar.readonly => On => On
phar.require_hash => On => On
%a
===DONE===
