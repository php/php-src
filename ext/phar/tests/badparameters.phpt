--TEST--
Phar: bad parameters to various methods
--EXTENSIONS--
phar
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
Phar::mungServer(): Argument #1 ($variables) must be of type array, string given
Phar::createDefaultStub(): Argument #1 ($index) must be of type ?string, array given
Phar::loadPhar(): Argument #1 ($filename) must be of type string, array given
Phar::canCompress(): Argument #1 ($compression) must be of type int, string given
Phar::__construct(): Argument #1 ($filename) must be of type string, array given
Phar::convertToExecutable(): Argument #1 ($format) must be of type ?int, array given
Phar::convertToData(): Argument #1 ($format) must be of type ?int, array given
PharData::delete(): Argument #1 ($localName) must be of type string, array given
Cannot write out phar archive, phar is read-only
Entry oops does not exist and cannot be deleted
%sfrontcontroller10.phar
Cannot write out phar archive, phar is read-only
A Phar alias cannot be set in a plain tar archive
Phar::setAlias(): Argument #1 ($alias) must be of type string, array given
Cannot change stub, phar is read-only
A Phar stub cannot be set in a plain tar archive
Phar::setStub(): Argument #1 ($stub) must be of type string, array given
A Phar stub cannot be set in a plain tar archive
Phar::setDefaultStub(): Argument #1 ($index) must be of type ?string, array given
Cannot change stub: phar.readonly=1
Cannot set signature algorithm, phar is read-only
Phar::compress(): Argument #1 ($compression) must be of type int, array given
Cannot compress phar archive, phar is read-only
Phar::compressFiles(): Argument #1 ($compression) must be of type int, array given
Phar is readonly, cannot change compression
Phar::copy() expects exactly 2 arguments, 1 given
Cannot copy "a" to "b", phar is read-only
Phar::offsetExists(): Argument #1 ($localName) must be of type string, array given
Phar::offsetGet(): Argument #1 ($localName) must be of type string, array given
Phar::offsetSet() expects exactly 2 arguments, 1 given
PharData::offsetUnset(): Argument #1 ($localName) must be of type string, array given
Write operations disabled by the php.ini setting phar.readonly
Phar::addEmptyDir(): Argument #1 ($directory) must be of type string, array given
Phar::addFile(): Argument #1 ($filename) must be of type string, array given
Phar::addFromString() expects exactly 2 arguments, 1 given
Write operations disabled by the php.ini setting phar.readonly
Phar::setMetadata() expects exactly 1 argument, 2 given
Write operations disabled by the php.ini setting phar.readonly
