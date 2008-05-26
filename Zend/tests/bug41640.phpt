--TEST--
Bug #41640 (get_class_vars produces error on class constants)
--FILE--
<?php
class foo {
        const FOO = 1;
        public $x = self::FOO;
}

var_dump(get_class_vars("foo"));
--EXPECT--
array(1) {
  [u"x"]=>
  int(1)
}
