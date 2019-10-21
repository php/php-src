--TEST--
ReflectionZendExtension: basic tests for accessors
--SKIPIF--
<?php
$zendExtensions = get_loaded_extensions(true);
if (!in_array('Zend OPcache', $zendExtensions)) {
    die('SKIP the Zend OPcache extension not available');
}
?>
--FILE--
<?php
$rze = new ReflectionZendExtension('Zend OPcache');

var_dump($rze->getName());
var_dump($rze->getVersion() === PHP_VERSION);
var_dump($rze->getAuthor());
var_dump($rze->getURL());
var_dump($rze->getCopyright());
?>
--EXPECT--
string(12) "Zend OPcache"
bool(true)
string(17) "Zend Technologies"
string(20) "http://www.zend.com/"
string(13) "Copyright (c)"
