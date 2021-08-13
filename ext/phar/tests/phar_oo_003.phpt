--TEST--
Phar object: entry & openFile()
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass();
foreach($phar as $name => $ent)
{
    var_dump($ent->getFilename());
    if ($ent->isDir()) {
        var_dump('DIR');
    } else {
        var_dump($ent->openFile()->fgets());
        include $ent->getPathName();
    }
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_003.phar.php');
__halt_compiler();
?>
--EXPECTF--
string(5) "a.php"
string(32) "<?php echo "This is a.php\n"; ?>"
This is a.php
string(1) "b"
%s(3) "DIR"
string(5) "b.php"
string(32) "<?php echo "This is b.php\n"; ?>"
This is b.php
string(5) "e.php"
string(32) "<?php echo "This is e.php\n"; ?>"
This is e.php
