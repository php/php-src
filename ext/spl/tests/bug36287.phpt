--TEST--
Bug #36287 (Segfault with SplFileInfo conversion)
--FILE--
<?php

$it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator("."), true);

$idx = 0;
foreach($it as $file)
{
    echo "First\n";
    var_Dump($file->getFilename());
    echo "Second\n";
    var_dump($file->getFilename());
    if (++$idx > 1)
    {
        break;
    }
}

?>
--EXPECTF--
First
string(%d) "%s"
Second
string(%d) "%s"
First
string(%d) "%s"
Second
string(%d) "%s"
