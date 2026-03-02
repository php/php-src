--TEST--
Phar object: iterating via SplFileObject and reading csv
--EXTENSIONS--
phar
--SKIPIF--
<?php if (!defined('SplFileObject::READ_CSV') || !defined('SplFileObject::SKIP_EMPTY')) die('skip newer SPL version is required'); ?>
--INI--
phar.require_hash=0
--FILE--
<?php

$pharconfig = 2;

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass('SplFileObject');

$f = $phar['a.csv'];
$f->setFlags(SplFileObject::SKIP_EMPTY | SplFileObject::DROP_NEW_LINE);
foreach($f as $k => $v)
{
    echo "$k=>$v\n";
}

?>
===CSV===
<?php

$f->setFlags(SplFileObject::SKIP_EMPTY | SplFileObject::DROP_NEW_LINE | SplFileObject::READ_CSV);
foreach($f as $k => $v)
{
    echo "$k=>" . join('|', $v) . "\n";
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_009.phar.php');
__halt_compiler();
?>
--EXPECT--
0=>1,2,3
1=>2,a,b
2=>3,"c","'e'"
3=>4
4=>5,5
5=>7,777
===CSV===
0=>1|2|3
1=>2|a|b
2=>3|c|'e'
3=>4
4=>5|5
6=>7|777
