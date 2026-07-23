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
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    Phar::createDefaultStub(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    Phar::loadPhar(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    Phar::canCompress('hi');
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a = new Phar(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a = new Phar(__DIR__ . '/files/frontcontroller10.phar');
} catch (PharException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->convertToExecutable(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->convertToData(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $b = new PharData(__DIR__ . '/whatever.tar');
} catch (PharException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $c = new PharData(__DIR__ . '/whatever.zip');
} catch (PharException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $b->delete(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->delete('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $b->delete('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo $a->getPath() . "\n";
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->setAlias('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $b->setAlias('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 0);
try {
    $a->setAlias(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 1);
try {
    $b->stopBuffering();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->setStub('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $b->setStub('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 0);
try {
    $a->setStub(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 1);
try {
    $b->setDefaultStub('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->setDefaultStub(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->setDefaultStub('oops');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->setSignatureAlgorithm(Phar::MD5);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->compress(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->compress(1);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->compressFiles(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->decompressFiles();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->copy(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->copy('a', 'b');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->offsetExists(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->offsetGet(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 0);
try {
    $a->offsetSet(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 1);
try {
    $b->offsetUnset(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->offsetUnset('a');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->addEmptyDir(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->addFile(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->addFromString(array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a->setMetadata('a');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 0);
try {
    $a->setMetadata(1,2);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 1);
try {
    $a->delMetadata();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
TypeError: Phar::mungServer(): Argument #1 ($variables) must be of type array, string given
TypeError: Phar::createDefaultStub(): Argument #1 ($index) must be of type ?string, array given
TypeError: Phar::loadPhar(): Argument #1 ($filename) must be of type string, array given
TypeError: Phar::canCompress(): Argument #1 ($compression) must be of type int, string given
TypeError: Phar::__construct(): Argument #1 ($filename) must be of type string, array given
TypeError: Phar::convertToExecutable(): Argument #1 ($format) must be of type ?int, array given
TypeError: Phar::convertToData(): Argument #1 ($format) must be of type ?int, array given
TypeError: PharData::delete(): Argument #1 ($localName) must be of type string, array given
UnexpectedValueException: Cannot write out phar archive, phar is read-only
BadMethodCallException: Entry oops does not exist and cannot be deleted
%sfrontcontroller10.phar
UnexpectedValueException: Cannot write out phar archive, phar is read-only
UnexpectedValueException: A Phar alias cannot be set in a plain tar archive
TypeError: Phar::setAlias(): Argument #1 ($alias) must be of type string, array given
UnexpectedValueException: Cannot change stub, phar is read-only
UnexpectedValueException: A Phar stub cannot be set in a plain tar archive
TypeError: Phar::setStub(): Argument #1 ($stub) must be of type string, array given
UnexpectedValueException: A Phar stub cannot be set in a plain tar archive
TypeError: Phar::setDefaultStub(): Argument #1 ($index) must be of type ?string, array given
UnexpectedValueException: Cannot change stub: phar.readonly=1
UnexpectedValueException: Cannot set signature algorithm, phar is read-only
TypeError: Phar::compress(): Argument #1 ($compression) must be of type int, array given
UnexpectedValueException: Cannot compress phar archive, phar is read-only
TypeError: Phar::compressFiles(): Argument #1 ($compression) must be of type int, array given
BadMethodCallException: Phar is readonly, cannot change compression
ArgumentCountError: Phar::copy() expects exactly 2 arguments, 1 given
UnexpectedValueException: Cannot copy "a" to "b", phar is read-only
TypeError: Phar::offsetExists(): Argument #1 ($localName) must be of type string, array given
TypeError: Phar::offsetGet(): Argument #1 ($localName) must be of type string, array given
ArgumentCountError: Phar::offsetSet() expects exactly 2 arguments, 1 given
TypeError: PharData::offsetUnset(): Argument #1 ($localName) must be of type string, array given
BadMethodCallException: Write operations disabled by the php.ini setting phar.readonly
TypeError: Phar::addEmptyDir(): Argument #1 ($directory) must be of type string, array given
TypeError: Phar::addFile(): Argument #1 ($filename) must be of type string, array given
ArgumentCountError: Phar::addFromString() expects exactly 2 arguments, 1 given
BadMethodCallException: Write operations disabled by the php.ini setting phar.readonly
ArgumentCountError: Phar::setMetadata() expects exactly 1 argument, 2 given
BadMethodCallException: Write operations disabled by the php.ini setting phar.readonly
