--TEST--
Bug #33136 (method offsetSet in class extended from ArrayObject crash PHP)
--FILE--
<?php

class Collection extends ArrayObject
{
	private $data;
	
	function __construct()
	{
		$this->data = array();
		parent::__construct($this->data);
	}
	
	function offsetGet($index)
	{
		echo __METHOD__ . "($index)\n";
		return parent::offsetGet($index);
	}
	
	function offsetSet($index, $value)
	{
		echo __METHOD__ . "(" . (is_null($index) ? "NULL" : $index) . ",$value)\n";
		parent::offsetSet($index, $value);
	}
}

echo "\n\nInitiate Obj\n";
$arrayObj = new Collection();

echo "Assign values\n";

$arrayObj[] = "foo";
var_dump($arrayObj[0]);

$arrayObj[] = "bar";
var_dump($arrayObj[0]);
var_dump($arrayObj[1]);

$arrayObj["foo"] = "baz";
var_dump($arrayObj["foo"]);

print_r($arrayObj);

var_dump(count($arrayObj));

?>
===DONE===
<?php //exit(0); ?>
--EXPECT--
Initiate Obj
Assign values
Collection::offsetSet(NULL,foo)
Collection::offsetGet(0)
string(3) "foo"
Collection::offsetSet(NULL,bar)
Collection::offsetGet(0)
string(3) "foo"
Collection::offsetGet(1)
string(3) "bar"
Collection::offsetSet(foo,baz)
Collection::offsetGet(foo)
string(3) "baz"
Collection Object
(
    [data:Collection:private] => Array
        (
        )

    [storage:ArrayObject:private] => Array
        (
            [0] => foo
            [1] => bar
            [foo] => baz
        )

)
int(3)
===DONE===
