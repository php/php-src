--TEST--
Phar: copy() tar-based
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar.php';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '2.phar.php';

$pname = 'phar://'.$fname;
$iname = '/file.txt';
$ename = '/error/..';

$p = new Phar($fname);

try
{
    $p['a'] = 'hi';
    $p->startBuffering();
    $p->copy('a', 'b');
    echo file_get_contents($p['b']->getPathName());
    $p->copy('b', 'c');
    $p->stopBuffering();
    echo file_get_contents($p['c']->getPathName());
    copy($fname, $fname2);
    var_dump($p->isFileFormat(Phar::TAR));
    $p->copy('a', $ename);
}
catch(Exception $e)
{
    echo $e->getMessage() . "\n";
}
ini_set('phar.readonly',1);
$p2 = new Phar($fname2);
var_dump($p2->isFileFormat(Phar::TAR));
echo "\n";
echo 'a: ' , file_get_contents($p2['a']->getPathName());
echo 'b: ' ,file_get_contents($p2['b']->getPathName());
echo 'c: ' ,file_get_contents($p2['c']->getPathName());
?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.php'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar.php'); ?>
--EXPECTF--
hihibool(true)
file "/error/.." contains invalid characters upper directory reference, cannot be copied from "a" in phar %s
bool(true)

a: hib: hic: hi===DONE===
