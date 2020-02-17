--TEST--
Phar: bad parameters to various methods
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
ini_set('phar.readonly', 1);

try {
    Phar::mungServer('hi');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    Phar::createDefaultStub(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    Phar::loadPhar(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    Phar::canCompress('hi');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a = new Phar(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a = new Phar(__DIR__ . '/files/frontcontroller10.phar');
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->convertToExecutable(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->convertToData(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $b = new PharData(__DIR__ . '/whatever.tar');
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $c = new PharData(__DIR__ . '/whatever.zip');
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $b->delete(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->delete('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $b->delete('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    echo $a->getPath() . "\n";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->setAlias('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $b->setAlias('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
try {
    $a->setAlias(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
ini_set('phar.readonly', 1);
try {
    $b->stopBuffering();
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->setStub('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $b->setStub('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
try {
    $a->setStub(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
ini_set('phar.readonly', 1);
try {
    $b->setDefaultStub('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->setDefaultStub(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->setDefaultStub('oops');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->setSignatureAlgorithm(Phar::MD5);
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->compress(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->compress(1);
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->compressFiles(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->decompressFiles();
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->copy(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->copy('a', 'b');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->offsetExists(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->offsetGet(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
ini_set('phar.readonly', 0);
try {
    $a->offsetSet(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
ini_set('phar.readonly', 1);
try {
    $b->offsetUnset(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->offsetUnset('a');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $a->addEmptyDir(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->addFile(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->addFromString(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a->setMetadata('a');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
try {
    $a->setMetadata(1,2);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
ini_set('phar.readonly', 1);
try {
    $a->delMetadata();
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Phar::mungServer() expects argument #1 ($munglist) to be of type array, string given
Phar::createDefaultStub() expects argument #1 ($index) to be a valid path, array given
Phar::loadPhar() expects argument #1 ($filename) to be a valid path, array given
Phar::canCompress() expects argument #1 ($method) to be of type int, string given
Phar::__construct() expects argument #1 ($filename) to be a valid path, array given
Phar::convertToExecutable() expects argument #1 ($format) to be of type int, array given
Phar::convertToData() expects argument #1 ($format) to be of type int, array given
PharData::delete() expects argument #1 ($entry) to be a valid path, array given
Cannot write out phar archive, phar is read-only
Entry oops does not exist and cannot be deleted
%sfrontcontroller10.phar
Cannot write out phar archive, phar is read-only
A Phar alias cannot be set in a plain tar archive
Phar::setAlias() expects argument #1 ($alias) to be of type string, array given
Cannot change stub, phar is read-only
A Phar stub cannot be set in a plain tar archive
Phar::setStub() expects argument #1 ($newstub) to be of type string, array given
A Phar stub cannot be set in a plain tar archive
Phar::setDefaultStub() expects argument #1 ($index) to be of type string, array given
Cannot change stub: phar.readonly=1
Cannot set signature algorithm, phar is read-only
Phar::compress() expects argument #1 ($compression_type) to be of type int, array given
Cannot compress phar archive, phar is read-only
Phar::compressFiles() expects argument #1 ($compression_type) to be of type int, array given
Phar is readonly, cannot change compression
Phar::copy() expects exactly 2 parameters, 1 given
Cannot copy "a" to "b", phar is read-only
Phar::offsetExists() expects argument #1 ($entry) to be a valid path, array given
Phar::offsetGet() expects argument #1 ($entry) to be a valid path, array given
Phar::offsetSet() expects exactly 2 parameters, 1 given
PharData::offsetUnset() expects argument #1 ($entry) to be a valid path, array given
Write operations disabled by the php.ini setting phar.readonly
Phar::addEmptyDir() expects argument #1 ($dirname) to be a valid path, array given
Phar::addFile() expects argument #1 ($filename) to be a valid path, array given
Phar::addFromString() expects exactly 2 parameters, 1 given
Write operations disabled by the php.ini setting phar.readonly
Phar::setMetadata() expects exactly 1 parameter, 2 given
Write operations disabled by the php.ini setting phar.readonly
