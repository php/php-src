--TEST--
Bug #33136 (method offsetSet in class extended from ArrayObject crash PHP)
--FILE--
<?php

class Collection extends ArrayObject
{
    private $data;

    function __construct()
    {
        $this->data = [];
        parent::__construct($this->data);
    }

    function offsetGet($index): mixed
    {
        echo __METHOD__ . "($index)\n";
        return parent::offsetGet($index);
    }

    function offsetSet($index, $value): void
    {
        echo __METHOD__ . "(" . (is_null($index) ? "NULL" : $index) . ",$value)\n";
        parent::offsetSet($index, $value);
    }
}

echo "Initiate Obj\n";
$arrayObj = new Collection();

echo "Assign values\n";

$arrayObj[] = "foo";
var_dump($arrayObj[0]);

$arrayObj[] = "bar";
var_dump($arrayObj[0]);
var_dump($arrayObj[1]);

$arrayObj["foo"] = "baz";
var_dump($arrayObj["foo"]);

var_dump($arrayObj);

var_dump(count($arrayObj));

?>
--EXPECT--
Initiate Obj
Assign values
Collection::offsetGet(0)
string(3) "foo"
Collection::offsetGet(0)
string(3) "foo"
Collection::offsetGet(1)
string(3) "bar"
Collection::offsetSet(foo,baz)
Collection::offsetGet(foo)
string(3) "baz"
object(Collection)#1 (2) {
  ["data":"Collection":private]=>
  array(0) {
  }
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
    ["foo"]=>
    string(3) "baz"
  }
}
int(3)
