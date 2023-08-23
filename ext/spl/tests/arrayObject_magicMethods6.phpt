--TEST--
SPL: ArrayObject: ensure the magic methods for property access of a subclass of ArrayObject are not invoked when manipulating its elements using -> ArrayObject::ARRAY_AS_PROPS.
--FILE--
<?php
class C {
    public $a = 1;
    public $b = 2;
    public $c = 3;

    private $priv = 'secret';
}

class UsesMagic extends ArrayObject {

    public $b = "This should never appear in storage";

    function __get($name) {
        $args = func_get_args();
        echo "In " . __METHOD__ . "(" . implode($args, ',') . ")\n";
    }
    function __set($name, $value) {
        $args = func_get_args();
        echo "In " . __METHOD__ . "(" . implode($args, ',') . ")\n";
    }
    function __isset($name) {
        $args = func_get_args();
        echo "In " . __METHOD__ . "(" . implode($args, ',') . ")\n";
    }
    function __unset($name) {
        $args = func_get_args();
        echo "In " . __METHOD__ . "(" . implode($args, ',') . ")\n";
    }

}
$obj = new C;
$ao = new UsesMagic($obj, ArrayObject::ARRAY_AS_PROPS);
echo "\n--> Write existent, non-existent and dynamic:\n";
$ao->a = 'changed';
$ao->dynamic = 'new';
$ao->dynamic = 'new.changed';
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);

echo "\n--> Read existent, non-existent and dynamic:\n";
var_dump($ao->a);
var_dump($ao->nonexistent);
var_dump($ao->dynamic);
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);

echo "\n--> isset existent, non-existent and dynamic:\n";
var_dump(isset($ao->a));
var_dump(isset($ao->nonexistent));
var_dump(isset($ao->dynamic));
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);

echo "\n--> Unset existent, non-existent and dynamic:\n";
unset($ao->a);
unset($ao->nonexistent);
unset($ao->dynamic);
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);
?>
--EXPECTF--
--> Write existent, non-existent and dynamic:

Deprecated: Creation of dynamic property C::$dynamic is deprecated in %s on line %d

Deprecated: Creation of dynamic property C::$dynamic is deprecated in %s on line %d
  Original wrapped object:
object(C)#1 (5) {
  ["a"]=>
  string(7) "changed"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"C":private]=>
  string(6) "secret"
  ["dynamic"]=>
  string(11) "new.changed"
}
  Wrapping ArrayObject:
object(UsesMagic)#2 (2) {
  ["b"]=>
  string(35) "This should never appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (5) {
    ["a"]=>
    string(7) "changed"
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
    ["dynamic"]=>
    string(11) "new.changed"
  }
}

--> Read existent, non-existent and dynamic:
string(7) "changed"

Warning: Undefined array key "nonexistent" in %s on line %d
NULL
string(11) "new.changed"
  Original wrapped object:
object(C)#1 (5) {
  ["a"]=>
  string(7) "changed"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"C":private]=>
  string(6) "secret"
  ["dynamic"]=>
  string(11) "new.changed"
}
  Wrapping ArrayObject:
object(UsesMagic)#2 (2) {
  ["b"]=>
  string(35) "This should never appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (5) {
    ["a"]=>
    string(7) "changed"
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
    ["dynamic"]=>
    string(11) "new.changed"
  }
}

--> isset existent, non-existent and dynamic:
bool(true)
bool(false)
bool(true)
  Original wrapped object:
object(C)#1 (5) {
  ["a"]=>
  string(7) "changed"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"C":private]=>
  string(6) "secret"
  ["dynamic"]=>
  string(11) "new.changed"
}
  Wrapping ArrayObject:
object(UsesMagic)#2 (2) {
  ["b"]=>
  string(35) "This should never appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (5) {
    ["a"]=>
    string(7) "changed"
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
    ["dynamic"]=>
    string(11) "new.changed"
  }
}

--> Unset existent, non-existent and dynamic:
  Original wrapped object:
object(C)#1 (3) {
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"C":private]=>
  string(6) "secret"
}
  Wrapping ArrayObject:
object(UsesMagic)#2 (2) {
  ["b"]=>
  string(35) "This should never appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (3) {
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
  }
}
