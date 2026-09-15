--TEST--
Preloading trait uses with aliased names
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--EXTENSIONS--
opcache
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
