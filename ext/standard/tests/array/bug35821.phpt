--TEST--
Bug #35821 (array_map() segfaults when exception is throwed from the callback)
--FILE--
<?php

class Element
{
	public function ThrowException ()
	{
		throw new Exception();
	}

	public static function CallBack(Element $elem)
	{
		$elem->ThrowException();
	}
}

$arr = array(new Element(), new Element(), new Element());
array_map(array('Element', 'CallBack'), $arr);

echo "Done\n";
?>
--EXPECTF--	

Exception: (empty message) in %s on line %d
Stack trace:
#0 %s(%d): Element->ThrowException()
#1 [internal function]: Element::CallBack(Object(Element))
#2 %s(%d): array_map(Array, Array)
#3 {main}
