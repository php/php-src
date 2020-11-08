--TEST--
Bug #67072 Echoing unserialized "SplFileObject" crash
--FILE--
<?php
    echo unserialize('O:13:"SplFileObject":1:{s:9:"*filename";s:15:"/home/flag/flag";}');
?>
===DONE==
--EXPECTF--
Warning: Erroneous data format for unserializing 'SplFileObject' in %sbug67072.php on line %d

Notice: unserialize(): Error at offset 24 of 64 bytes in %sbug67072.php on line %d
===DONE==
