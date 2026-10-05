--TEST--
Error recording is disabled after a fatal error during the optimizer with opcache.file_cache_only
--EXTENSIONS--
opcache
zend_test
--SKIPIF--
<?php
if (getenv("SKIP_REPEAT")) die("skip Not compatible with repeat");
if (getenv("SKIP_PRELOAD")) die("skip Not compatible with preload");
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
zend_test.register_passes=1
opcache.file_cache="{TMP}"
opcache.file_cache_only=1
--FILE--
<?php
register_shutdown_function(function () {
    trigger_error('From shutdown', E_USER_WARNING);
});
ini_set('zend_test.fatal_error_in_pass', '1');
require __DIR__ . '/bailout.inc';
?>
--EXPECTF--
%Apass1

Fatal error: Fatal error in pass1 in %s on line %d

Warning: From shutdown in %s on line %d
