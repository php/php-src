--TEST--
Preloading trait uses with aliased names
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(get_class_methods('TraitAliasTest'));
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(3) "foo"
}
