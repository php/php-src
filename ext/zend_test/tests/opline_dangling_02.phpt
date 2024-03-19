--TEST--
possible segfault in `ZEND_FUNC_GET_ARGS`
--EXTENSIONS--
zend_test
--INI--
zend_test.observe_opline_in_zendmm=1
--FILE--
<?php

function ref() {
    return func_get_args();
}

class Foo {
    public static int $i;
    public static string $s = "x";
}

var_dump(Foo::$i = "1");
var_dump(Foo::$s, Foo::$i);
var_dump(ref('string', 0));

echo 'Done.';
?>
--EXPECT--
int(1)
string(1) "x"
int(1)
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  int(0)
}
Done.
