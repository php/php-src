--TEST--
Phar: tar archive, require_hash=1, should not error out
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=1
phar.require_hash=1
--FILE--
<?php
try {
    $phar = new PharData(__DIR__ . '/files/Net_URL-1.0.15.tgz');
    var_dump($phar->getStub());
} catch (Exception $e) {
    echo $e->getMessage()."\n";
}

?>
--EXPECT--
string(0) ""
