--TEST--
Phar: bad parameters to various methods
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
ini_set('phar.readonly', 1);
Phar::mungServer('hi');
Phar::createDefaultStub(array());
Phar::loadPhar(array());
Phar::canCompress('hi');
$a = new Phar(array());
$a = new Phar(dirname(__FILE__) . '/files/frontcontroller10.phar');
$a->convertToExecutable(array());
$a->convertToData(array());
$b = new PharData(dirname(__FILE__) . '/whatever.tar');
$c = new PharData(dirname(__FILE__) . '/whatever.zip');
$b->delete(array());
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
echo $a->getPath() . "\n";
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
$a->setAlias(array());
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
$a->setStub(array());
ini_set('phar.readonly', 1);
try {
$b->setDefaultStub('oops');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$a->setDefaultStub(array());
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
$a->compress(array());
try {
$a->compress(1);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$a->compressFiles(array());
try {
$a->decompressFiles();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$a->copy(array());
try {
$a->copy('a', 'b');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$a->offsetExists(array());
$a->offsetGet(array());
ini_set('phar.readonly', 0);
$a->offsetSet(array());
ini_set('phar.readonly', 1);
$b->offsetUnset(array());
try {
$a->offsetUnset('a');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$a->addEmptyDir(array());
$a->addFile(array());
$a->addFromString(array());
try {
$a->setMetadata('a');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
$a->setMetadata(1,2);
ini_set('phar.readonly', 1);
try {
$a->delMetadata();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Warning: Phar::mungServer() expects parameter 1 to be array, %string given in %sbadparameters.php on line %d

Warning: Phar::createDefaultStub() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::loadPhar() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::canCompress() expects parameter 1 to be integer, %string given in %sbadparameters.php on line %d

Warning: Phar::__construct() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::convertToExecutable() expects parameter 1 to be integer, array given in %sbadparameters.php on line %d

Warning: Phar::convertToData() expects parameter 1 to be integer, array given in %sbadparameters.php on line %d

Warning: PharData::delete() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d
Cannot write out phar archive, phar is read-only
Entry oops does not exist and cannot be deleted
%sfiles/frontcontroller10.phar
Cannot write out phar archive, phar is read-only
A Phar alias cannot be set in a plain tar archive

Warning: Phar::setAlias() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d
Cannot change stub, phar is read-only
A Phar stub cannot be set in a plain tar archive

Warning: Phar::setStub() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d
A Phar stub cannot be set in a plain tar archive

Warning: Phar::setDefaultStub() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d
Cannot change stub: phar.readonly=1
Cannot set signature algorithm, phar is read-only

Warning: Phar::compress() expects parameter 1 to be integer, array given in %sbadparameters.php on line %d
Cannot compress phar archive, phar is read-only

Warning: Phar::compressFiles() expects parameter 1 to be integer, array given in %sbadparameters.php on line %d
Phar is readonly, cannot change compression

Warning: Phar::copy() expects exactly 2 parameters, 1 given in %sbadparameters.php on line %d
Cannot copy "a" to "b", phar is read-only

Warning: Phar::offsetExists() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::offsetGet() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::offsetSet() expects exactly 2 parameters, 1 given in %sbadparameters.php on line %d

Warning: PharData::offsetUnset() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d
Write operations disabled by the php.ini setting phar.readonly

Warning: Phar::addEmptyDir() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::addFile() expects parameter 1 to be %string, array given in %sbadparameters.php on line %d

Warning: Phar::addFromString() expects exactly 2 parameters, 1 given in %sbadparameters.php on line %d
Write operations disabled by the php.ini setting phar.readonly

Warning: Phar::setMetadata() expects exactly 1 parameter, 2 given in %sbadparameters.php on line %d
Write operations disabled by the php.ini setting phar.readonly
===DONE===
