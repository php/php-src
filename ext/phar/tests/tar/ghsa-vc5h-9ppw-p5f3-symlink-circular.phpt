--TEST--
GHSA-vc5h-9ppw-p5f3 (circular symlinks in tar should not cause stack overflow)
--CREDITS--
Calvin Young - eWalker Consulting (HK) Limited
Enoch Chow - Isomorph Cyber
--EXTENSIONS--
phar
--FILE--
<?php
$base = dirname(__FILE__);

// simple 2-cycle
$phar = new PharData($base . '/files/circular_symlinks.tar');
var_dump($phar['file_a']->getContent() === '');

// rho-shaped cycle (tail leading into a loop)
$phar = new PharData($base . '/files/circular_symlinks_rho.tar');
var_dump($phar['file_a']->getContent() === '');

// long cycle (400 entries)
$phar = new PharData($base . '/files/circular_symlinks_long.tar');
var_dump($phar['link_0']->getContent() === '');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
