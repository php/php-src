--TEST--
GH-21264: Missing reference unwrap for FE_FETCH_R in JIT
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
--FILE--
<?php

class C {
    public array $array;

    public static function identity($x) {
        return $x;
    }

    public function test() {
        return array_map(self::identity(...), $this->array);
    }
}

function test() {
    $c = new C;
    $element = 'qux';
    $c->array = [&$element, &$element];
    var_dump($c->test());
}

test();
test();

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "qux"
  [1]=>
  string(3) "qux"
}
array(2) {
  [0]=>
  string(3) "qux"
  [1]=>
  string(3) "qux"
}
