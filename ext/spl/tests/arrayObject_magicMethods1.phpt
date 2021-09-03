--TEST--
SPL: ArrayObject: ensure a wrapped object's magic methods for property access are not invoked when manipulating the ArrayObject's elements using [].
--FILE--
<?php
class UsesMagic {
    public $a = 1;
    public $b = 2;
    public $c = 3;

    private $priv = 'secret';

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

$obj = new UsesMagic;

$ao = new ArrayObject($obj);
echo "\n--> Write existent, non-existent and dynamic:\n";
$ao['a'] = 'changed';
$ao['dynamic'] = 'new';
$ao['dynamic'] = 'new.changed';
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);

echo "\n--> Read existent, non-existent and dynamic:\n";
var_dump($ao['a']);
var_dump($ao['nonexistent']);
var_dump($ao['dynamic']);
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);

echo "\n--> isset existent, non-existent and dynamic:\n";
var_dump(isset($ao['a']));
var_dump(isset($ao['nonexistent']));
var_dump(isset($ao['dynamic']));
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);

echo "\n--> Unset existent, non-existent and dynamic:\n";
unset($ao['a']);
unset($ao['nonexistent']);
unset($ao['dynamic']);
echo "  Original wrapped object:\n";
var_dump($obj);
echo "  Wrapping ArrayObject:\n";
var_dump($ao);
?>
--EXPECTF--
--> Write existent, non-existent and dynamic:
  Original wrapped object:
object(UsesMagic)#1 (5) {
  ["a"]=>
  string(7) "changed"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"UsesMagic":private]=>
  string(6) "secret"
  ["dynamic"]=>
  string(11) "new.changed"
}
  Wrapping ArrayObject:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(UsesMagic)#1 (5) {
    ["a"]=>
    string(7) "changed"
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"UsesMagic":private]=>
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
object(UsesMagic)#1 (5) {
  ["a"]=>
  string(7) "changed"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"UsesMagic":private]=>
  string(6) "secret"
  ["dynamic"]=>
  string(11) "new.changed"
}
  Wrapping ArrayObject:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(UsesMagic)#1 (5) {
    ["a"]=>
    string(7) "changed"
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"UsesMagic":private]=>
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
object(UsesMagic)#1 (5) {
  ["a"]=>
  string(7) "changed"
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"UsesMagic":private]=>
  string(6) "secret"
  ["dynamic"]=>
  string(11) "new.changed"
}
  Wrapping ArrayObject:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(UsesMagic)#1 (5) {
    ["a"]=>
    string(7) "changed"
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"UsesMagic":private]=>
    string(6) "secret"
    ["dynamic"]=>
    string(11) "new.changed"
  }
}

--> Unset existent, non-existent and dynamic:
  Original wrapped object:
object(UsesMagic)#1 (3) {
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["priv":"UsesMagic":private]=>
  string(6) "secret"
}
  Wrapping ArrayObject:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(UsesMagic)#1 (3) {
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"UsesMagic":private]=>
    string(6) "secret"
  }
}
