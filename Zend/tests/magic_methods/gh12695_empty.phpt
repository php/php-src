--TEST--
GH-12695: empty() also benefits from the post-__isset() materialization re-check
--FILE--
<?php

#[AllowDynamicProperties]
class A {
    public function __get($n) {
        throw new Exception("__get must not be called when __isset materialised the property");
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        $this->$n = $GLOBALS['next_value'];
        return true;
    }
}

echo "1) empty() when __isset materialised a truthy value: __get is not called, empty=false:\n";
$GLOBALS['next_value'] = 7;
$a = new A;
var_dump(empty($a->foo));

echo "\n2) empty() when __isset materialised a falsy value: __get is not called, empty=true:\n";
$GLOBALS['next_value'] = 0;
$a = new A;
var_dump(empty($a->bar));

echo "\n3) empty() with no materialization: __get is still called (legacy path preserved):\n";
class B {
    public function __get($n) {
        echo "  __get($n)\n";
        return 'value';
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        return true;
    }
}
$b = new B;
var_dump(empty($b->any));

?>
--EXPECT--
1) empty() when __isset materialised a truthy value: __get is not called, empty=false:
  __isset(foo)
bool(false)

2) empty() when __isset materialised a falsy value: __get is not called, empty=true:
  __isset(bar)
bool(true)

3) empty() with no materialization: __get is still called (legacy path preserved):
  __isset(any)
  __get(any)
bool(false)
