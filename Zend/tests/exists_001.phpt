--TEST--
Testing exists()
--FILE--
<?php

function check($msg,$val,$ref)
{
if ($val===$ref) {
	echo "-- OK: $msg\n";
} else {
	echo "**** FAIL **** $msg (result -> ";
	var_dump($val);
	echo ")\n";
}
}

//-------------------------

class TestClass1 implements ArrayAccess
{
	private $a;
	public $c=null;

	public function __construct()
	{
		$this->a=array('4' => 5, 'foo' => 'bar', 'ind' => null, 'i2' => null);
	}

	public function offsetExists($offset)
	{
		return (($offset === 0) ? exists($this->b[0]) : exists($this->a[$offset]));
	}

	public function offsetGet($offset) {} //Unused
	public function offsetSet($offset,$value) {} // Unused

	public function offsetUnset($offset)
	{
		unset($this->a[$offset]);
	}

	public function __isset($key)
	{
		return exists($this->a[$key]);
	}
}

//-------------------------

check('Var does not exist', exists($var), false);

$var=null;
check('Var exists (null)', exists($var), true);

$var=3;
check('Var exists (non null)', exists($var), true);

unset($var);
check('Unset var does not exist', exists($var), false);

//-- Variable variable

$var=null;
$vv='var';
check('Variable var exists (null)', exists($$vv), true);
$vv='b';
check('Unset variable var does not exist', exists($$vv), false);

//-- Array

check('Array: Var does not exist (key=num, array does not exist)', exists($var[3]), false);

$var=array();
check('Array: Elt does not exist (key=num)', exists($var[3]), false);

check('Array: Elt does not exist (key=string)', exists($var['foo']), false);

$var[3]=null;
check('Array: Elt exists (null, key=num)', exists($var[3]), true);

$var['foo']=null;
check('Array: Elt exists (null, key=string)', exists($var['foo']), true);

$var[3]=4;
check('Array: Elt exists (non null, key=num)', exists($var[3]), true);

$var['foo']=4;
check('Array: Elt exists (non null, key=string)', exists($var['foo']), true);

unset($var[3]);
check('Array: Unset elt does not exist (key=num)\n', exists($var[3]), false);

unset($var['foo']);
check('Array: Unset elt does not exist (key=string)\n', exists($var['foo']), false);

unset($var);
check('Array: Element from unset array does not exist (key=num)', exists($var[3]), false);

//-- String offsets

$str="abcdef";

check('String: Valid offset', exists($str[3]), true);
check('String: Invalid offset (using [])', exists($str[10]), false);
check('String: Invalid offset (using {})', exists($str{10}), false);

//-- ArrayAccess

$t=new TestClass1;

check('ArrayAccess: undef property', exists($t[0]), false);
check('ArrayAccess: undef property (num)', exists($t[1]), false);
check('ArrayAccess: undef property (string)', exists($t['undef']), false);

check('ArrayAccess: defined property (num)', exists($t[4]), true);
check('ArrayAccess: defined property (string)', exists($t['foo']), true);

check('ArrayAccess: null defined property (string)', exists($t['ind']), true);
check('ArrayAccess: null exists using __isset (string)', exists($t->ind), true);
check('ArrayAccess: null isset using __isset (string)', isset($t->ind), true);

unset ($t[4]);
check('ArrayAccess: unset property (num)', exists($t[4]), false);

unset ($t['ind']);
check('ArrayAccess: unset null property (string)', exists($t['ind']), false);
check('ArrayAccess: unset null using __isset (string)', exists($t->ind), false);

//-- Properties


check('Properties: null static property is not set()', isset($t->c), false);
check('Properties: null static property exists', exists($t->c), true);

check('Properties: null dynamic property exists', exists($t->i2), true);
check('Properties: undef dynamic property does not exist', exists($t->undef), false);

print "done\n";

--EXPECT--
-- OK: Var does not exist
-- OK: Var exists (null)
-- OK: Var exists (non null)
-- OK: Unset var does not exist
-- OK: Variable var exists (null)
-- OK: Unset variable var does not exist
-- OK: Array: Var does not exist (key=num, array does not exist)
-- OK: Array: Elt does not exist (key=num)
-- OK: Array: Elt does not exist (key=string)
-- OK: Array: Elt exists (null, key=num)
-- OK: Array: Elt exists (null, key=string)
-- OK: Array: Elt exists (non null, key=num)
-- OK: Array: Elt exists (non null, key=string)
-- OK: Array: Unset elt does not exist (key=num)\n
-- OK: Array: Unset elt does not exist (key=string)\n
-- OK: Array: Element from unset array does not exist (key=num)
-- OK: String: Valid offset
-- OK: String: Invalid offset (using [])
-- OK: String: Invalid offset (using {})
-- OK: ArrayAccess: undef property
-- OK: ArrayAccess: undef property (num)
-- OK: ArrayAccess: undef property (string)
-- OK: ArrayAccess: defined property (num)
-- OK: ArrayAccess: defined property (string)
-- OK: ArrayAccess: null defined property (string)
-- OK: ArrayAccess: null exists using __isset (string)
-- OK: ArrayAccess: null isset using __isset (string)
-- OK: ArrayAccess: unset property (num)
-- OK: ArrayAccess: unset null property (string)
-- OK: ArrayAccess: unset null using __isset (string)
-- OK: Properties: null static property is not set()
-- OK: Properties: null static property exists
-- OK: Properties: null dynamic property exists
-- OK: Properties: undef dynamic property does not exist
done
