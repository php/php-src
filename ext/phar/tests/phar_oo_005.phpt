--TEST--
Phar and RecursiveDirectoryIterator
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';
$fname = str_replace('\\', '/', $fname);

$it = new RecursiveDirectoryIterator('phar://'.$fname);
$it = new RecursiveIteratorIterator($it);

foreach($it as $name => $ent)
{
    var_dump(str_replace(array('\\', $fname), array('/', '*'), $name));
    var_dump(str_replace(array('\\', $fname), array('/', '*'), $ent->getPathname()));
    var_dump(str_replace('\\', '/', $it->getSubPath()));
    var_dump(str_replace('\\', '/', $it->getSubPathName()));
    $sub = $it->getPathInfo();
    var_dump(str_replace('\\', '/', $sub->getFilename()));
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_005.phar.php');
__halt_compiler();
?>
--EXPECT--
string(14) "phar://*/a.php"
string(14) "phar://*/a.php"
string(0) ""
string(5) "a.php"
string(20) "phar_oo_005.phar.php"
string(16) "phar://*/b/c.php"
string(16) "phar://*/b/c.php"
string(1) "b"
string(7) "b/c.php"
string(1) "b"
string(16) "phar://*/b/d.php"
string(16) "phar://*/b/d.php"
string(1) "b"
string(7) "b/d.php"
string(1) "b"
string(14) "phar://*/b.php"
string(14) "phar://*/b.php"
string(0) ""
string(5) "b.php"
string(20) "phar_oo_005.phar.php"
string(14) "phar://*/e.php"
string(14) "phar://*/e.php"
string(0) ""
string(5) "e.php"
string(20) "phar_oo_005.phar.php"
