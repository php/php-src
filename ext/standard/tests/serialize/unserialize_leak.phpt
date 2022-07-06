--TEST--
Unserialize leak in SplObjectStorage
--FILE--
<?php

$payload = 'C:16:"SplObjectStorage":113:{x:i:2;O:8:"stdClass":1:{},a:2:{s:4:"prev";i:2;s:4:"next";O:8:"stdClass":0:{}};r:7;,R:2;s:4:"next";;r:3;};m:a:0:{}}';
try {
    var_dump(unserialize($payload));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Notice: SplObjectStorage::unserialize(): Unexpected end of serialized data in %s on line %d
Error at offset 24 of 113 bytes
