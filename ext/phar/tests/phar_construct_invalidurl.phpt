--TEST--
Phar object passed URL
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--FILE--
<?php
try {
    $a = new Phar('http://should.fail.com');
} catch (UnexpectedValueException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $a = new Phar('http://');
} catch (UnexpectedValueException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $a = new Phar('http:/');
} catch (UnexpectedValueException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
UnexpectedValueException: Cannot create a phar archive from a URL like "http://should.fail.com". Phar objects can only be created from local files
UnexpectedValueException: Cannot create a phar archive from a URL like "http://". Phar objects can only be created from local files
UnexpectedValueException: Cannot create phar 'http:/', file extension (or combination) not recognised or the directory does not exist
