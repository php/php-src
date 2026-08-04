--TEST--
Bug #72434: ZipArchive class Use After Free Vulnerability in PHP's GC algorithm and unserialize
--EXTENSIONS--
zip
--FILE--
<?php
$free_me = array(new StdClass());
$serialized_payload = 'a:3:{i:1;N;i:2;O:10:"ZipArchive":1:{s:8:"filename";'.serialize($free_me).'}i:1;R:4;}';
try {
    $unserialized_payload = unserialize($serialized_payload);
    var_dump($unserialized_payload);
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Unserialization of 'ZipArchive' is not allowed, override __serialize() and __unserialize() to implement it
