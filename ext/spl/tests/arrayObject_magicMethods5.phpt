--TEST--
SPL: ArrayObject: ensure the magic methods for property access of a subclass of ArrayObject ARE invoked when manipulating its elements using ->.
--FILE--
<?php
class C {
	public $a = 1;
	public $b = 2;
	public $c = 3;

	private $priv = 'secret';
}

class UsesMagic extends ArrayObject {

	public $b = "This should appear in storage";

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
$ao = new UsesMagic($obj);
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
In UsesMagic::__set(a,changed)
In UsesMagic::__set(dynamic,new)
In UsesMagic::__set(dynamic,new.changed)
  Original wrapped object:
object(C)#1 (4) {
  ["a"]=>
  int(1)
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
  string(29) "This should appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
  }
}

--> Read existent, non-existent and dynamic:
In UsesMagic::__get(a)
NULL
In UsesMagic::__get(nonexistent)
NULL
In UsesMagic::__get(dynamic)
NULL
  Original wrapped object:
object(C)#1 (4) {
  ["a"]=>
  int(1)
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
  string(29) "This should appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
  }
}

--> isset existent, non-existent and dynamic:
In UsesMagic::__isset(a)
bool(false)
In UsesMagic::__isset(nonexistent)
bool(false)
In UsesMagic::__isset(dynamic)
bool(false)
  Original wrapped object:
object(C)#1 (4) {
  ["a"]=>
  int(1)
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
  string(29) "This should appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
  }
}

--> Unset existent, non-existent and dynamic:
In UsesMagic::__unset(a)
In UsesMagic::__unset(nonexistent)
In UsesMagic::__unset(dynamic)
  Original wrapped object:
object(C)#1 (4) {
  ["a"]=>
  int(1)
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
  string(29) "This should appear in storage"
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["priv":"C":private]=>
    string(6) "secret"
  }
}
