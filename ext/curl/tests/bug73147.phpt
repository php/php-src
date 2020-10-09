--TEST--
Bug #73147: Use After Free in PHP7 unserialize()
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

$poc = 'a:1:{i:0;O:8:"CURLFile":1:{s:4:"name";R:1;}}';
try {
    var_dump(unserialize($poc));
} catch(Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
Warning: Erroneous data format for unserializing 'CURLFile' in %s on line %d

Notice: unserialize(): Error at offset 27 of 44 bytes in %s on line %d
bool(false)
