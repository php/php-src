--TEST--
Phar: phpinfo display 1
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not loaded"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not loaded"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
ini_set('phar.extract_list', 'phar_test.phar='.dirname(__FILE__).',another.phar='. dirname(__FILE__));
phpinfo(INFO_MODULES);
ini_set('phar.readonly',1);
ini_set('phar.require_hash',1);
ini_set('phar.extract_list', '');
phpinfo(INFO_MODULES);
?>
===DONE===
--EXPECTF--
%aPhar

Phar: PHP Archive support => enabled
Phar EXT version => %s
Phar API version => 1.1.1
CVS revision => %sRevision: %s $
Phar-based phar archives => enabled
Tar-based phar archives => enabled
ZIP-based phar archives => enabled
gzip compression => enabled
bzip2 compression => enabled


Phar based on pear/PHP_Archive, original concept by Davey Shafik.
Phar fully realized by Gregory Beaver and Marcus Boerger.
Portions of tar implementation Copyright (c) 2003-2007 Tim Kientzle.
Directive => Local Value => Master Value
phar.extract_list => [phar_test.phar = %stests],[another.phar = %stests] => 
phar.readonly => Off => Off
phar.require_hash => Off => Off
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


Phar based on pear/PHP_Archive, original concept by Davey Shafik.
Phar fully realized by Gregory Beaver and Marcus Boerger.
Portions of tar implementation Copyright (c) 2003-2007 Tim Kientzle.
Directive => Local Value => Master Value
phar.extract_list =>  => 
phar.readonly => On => Off
phar.require_hash => On => Off
%a
===DONE===
