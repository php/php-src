--TEST--
Bug #76943 (Inconsistent stream_wrapper_restore() errors)
--SKIPIF--
<?php
if (!in_array('phar', stream_get_wrappers())) die('skip phar wrapper not registered');
?>
--FILE--
<?php
var_dump(stream_wrapper_restore('foo'));
var_dump(stream_wrapper_restore('phar'));

stream_wrapper_register('bar', 'stdClass');

var_dump(stream_wrapper_restore('foo'));
var_dump(stream_wrapper_restore('phar'));
?>
--EXPECTF--
Warning: stream_wrapper_restore(): foo:// never existed, nothing to restore in %s on line %d
bool(false)

Notice: stream_wrapper_restore(): phar:// was never changed, nothing to restore in %s on line %d
bool(true)

Warning: stream_wrapper_restore(): foo:// never existed, nothing to restore in %s on line %d
bool(false)

Notice: stream_wrapper_restore(): phar:// was never changed, nothing to restore in %s on line %d
bool(true)
