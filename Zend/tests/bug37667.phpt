--TEST--
Bug #37667 (Object is not added into array returned by __get)
--FILE--
<?php

class Test
{
	protected $property = array('foo' => 'bar');

	function __get($name)
	{
		return $this->property;
	}
}

$obj = new Test;

var_dump($obj->property['foo']);
var_dump($obj->property[2]);

var_dump($obj);

$obj->property[] = 1;
$obj->property[] = 2;

var_dump($obj);

?>
===DONE===
--EXPECTF--
string(3) "bar"

Notice: Undefined offset:  2 in %sbug37667.php on line %d
NULL
object(Test)#%d (1) {
  ["property":protected]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}

Fatal error: Cannot use array returned from Test::__get('property') in write context in %sbug37667.php on line %d
--UEXPECTF--
unicode(3) "bar"

Notice: Undefined offset:  2 in %sbug37667.php on line %d
NULL
object(Test)#%d (1) {
  [u"property":protected]=>
  array(1) {
    [u"foo"]=>
    unicode(3) "bar"
  }
}

Fatal error: Cannot use array returned from Test::__get('property') in write context in %sbug37667.php on line %d
