--TEST--
Phar: create a completely new tar-based phar
--EXTENSIONS--
phar
--INI--
phar.readonly=1
phar.require_hash=1
--FILE--
<?php

file_put_contents('phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar/a.php',
    'brand new!');
include 'phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar/a.php';
?>
--EXPECTF--
Warning: file_put_contents(phar://%screate_new_phar_b.phar.tar/a.php): Failed to open stream: phar error: write operations disabled by the php.ini setting phar.readonly in %screate_new_phar_b.php on line %d

Warning: include(phar://%screate_new_phar_b.phar.tar/a.php): Failed to open stream: %s in %screate_new_phar_b.php on line %d

Warning: include(): Failed opening 'phar://%screate_new_phar_b.phar.tar/a.php' for inclusion (include_path='%s') in %screate_new_phar_b.php on line %d

