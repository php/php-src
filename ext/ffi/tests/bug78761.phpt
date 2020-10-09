--TEST--
Bug #78761 (Zend memory heap corruption with preload and casting)
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--INI--
opcache.enable_cli=1
opcache.preload={PWD}/bug78761_preload.php
--FILE--
<?php
try {
    FFI::cast('char[10]', FFI::new('char[1]'));
} catch (FFI\Exception $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
attempt to cast to larger type
