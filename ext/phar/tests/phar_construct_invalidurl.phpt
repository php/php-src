--TEST--
Phar object passed URL
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
    $a = new Phar('http://should.fail.com');
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(),"\n";
}
try {
    $a = new Phar('http://');
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(),"\n";
}
try {
    $a = new Phar('http:/');
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(),"\n";
}
?>
--EXPECT--
Cannot create a phar archive from a URL like "http://should.fail.com". Phar objects can only be created from local files
Cannot create a phar archive from a URL like "http://". Phar objects can only be created from local files
Cannot create phar 'http:/', file extension (or combination) not recognised or the directory does not exist
