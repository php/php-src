--TEST--
GH-12695: empty() on unset property does not call __get() when __isset() materialized the property
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

echo "empty() when __isset materialised a truthy value: __get is not called, empty=false:\n";
$GLOBALS['next_value'] = 7;
$a = new A;
var_dump(empty($a->foo));

echo "\nempty() when __isset materialised a falsy value: __get is not called, empty=true:\n";
$GLOBALS['next_value'] = 0;
$a = new A;
var_dump(empty($a->bar));

echo "\nempty() with no materialization: __get is still called (legacy path preserved):\n";
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
empty() when __isset materialised a truthy value: __get is not called, empty=false:
  __isset(foo)
bool(false)

empty() when __isset materialised a falsy value: __get is not called, empty=true:
  __isset(bar)
bool(true)

empty() with no materialization: __get is still called (legacy path preserved):
  __isset(any)
  __get(any)
bool(false)
