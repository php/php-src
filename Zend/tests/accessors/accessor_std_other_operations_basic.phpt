--TEST--
ZE2 Tests that a getter/setter works properly with other edge case uses 
--FILE--
<?php

class TimePeriod {
	
	public $test = 'jargon';
		
	public $tArray  {
		get { return $this->tArray; }
		set { $this->tArray = $value; }
	}
	public $tArray_ByRef  {
		&get { return $this->tArray_ByRef; }
		set { $this->tArray_ByRef = $value; }
	}
	
	public $objTest_ByRef {
		&get { return $this->objTest_ByRef; }
		set { $this->objTest_ByRef = $value; }
	}
	
	public $str_ByRef {
		&get { return $this->str_ByRef; }
		set { $this->str_ByRef = $value; }
	}
}

function Title($s) {
	echo PHP_EOL.PHP_EOL.$s.PHP_EOL;
	echo str_repeat('-', strlen($s)).PHP_EOL;
}

$o = new TimePeriod();
Title("** Array Reference Tests **");

	Title("Attempt to append to a non-by-reference array accessor:");
		$o->tArray = array();
		print_r($o->tArray);
		echo "\$o->tArray[] = 1".PHP_EOL;
			$o->tArray[] = 1;
		print_r($o->tArray);


	Title("Append to an array-by-ref:");
		echo '$o->tArray_ByRef = array(\'initial set\');'.PHP_EOL;
			$o->tArray_ByRef = array('initial set');
		echo 'print_r($o->tArray_ByRef);'.PHP_EOL;
			print_r($o->tArray_ByRef);
		echo "\$o->tArray_ByRef[] = 1".PHP_EOL;
			$o->tArray_ByRef[] = 1;
		print_r($o->tArray_ByRef);


	Title("Direct set to an array-by-ref:");
		echo '$o->tArray_ByRef = array(\'initial set 2\');'.PHP_EOL;
			$o->tArray_ByRef = array('initial set 2');
		echo 'print_r($o->tArray_ByRef);'.PHP_EOL;
			print_r($o->tArray_ByRef);
		echo "\$o->tArray_ByRef[] = 1".PHP_EOL;
			$o->tArray_ByRef[123] = 4;
		print_r($o->tArray_ByRef);


	Title("Copy reference to an array, modify reference copy, check original:");
		echo '$o->tArray_ByRef = array(\'initial set 3\');'.PHP_EOL;
			$o->tArray_ByRef = array('initial set 3');
		echo 'print_r($o->tArray_ByRef);'.PHP_EOL;
			print_r($o->tArray_ByRef);
		echo '$foobar = &$o->tArray_ByRef;'.PHP_EOL; 
			$foobar = &$o->tArray_ByRef;
		echo 'print_r($foobar);'.PHP_EOL;
			print_r($foobar);
		echo '$foobar[123] = 6;'.PHP_EOL;
			$foobar[123] = 6;
		echo 'print_r($foobar);'.PHP_EOL;
			print_r($foobar);
		echo 'print_r($o->tArray_ByRef);'.PHP_EOL;
			print_r($o->tArray_ByRef);

Title("** Object Tests **");

	Title("Set property of sub-object:");
		echo '$o->objTest_ByRef = new stdClass();'.PHP_EOL;
			$o->objTest_ByRef = new stdClass();
		echo 'print_r($o->objTest_ByRef);'.PHP_EOL;
			print_r($o->objTest_ByRef);
		echo '$o->objTest_ByRef->baz = 1;'.PHP_EOL;
			$o->objTest_ByRef->baz = 1;
		echo 'print_r($o->objTest_ByRef);'.PHP_EOL;
			print_r($o->objTest_ByRef);
	

	Title("Set property of empty property as though it were an object:");
		echo 'unset($o->objTest_ByRef);'.PHP_EOL;
			unset($o->objTest_ByRef);
		echo 'print_r($o->objTest_ByRef);'.PHP_EOL;
			print_r($o->objTest_ByRef);
		echo '$o->objTest_ByRef->baz = 1;'.PHP_EOL;
			$o->objTest_ByRef->baz = 1;
		echo 'print_r($o->objTest_ByRef);'.PHP_EOL;
			print_r($o->objTest_ByRef);

	Title("Set property of string property as though it were an object:");
		echo '$o->objTest_ByRef = "string".PHP_EOL;'.PHP_EOL;
			$o->objTest_ByRef = "string".PHP_EOL;
		echo 'print_r($o->objTest_ByRef);'.PHP_EOL;
			print_r($o->objTest_ByRef);
		echo '$o->objTest_ByRef->baz = 1;'.PHP_EOL;
			$o->objTest_ByRef->baz = 1;
		echo 'print_r($o->objTest_ByRef);'.PHP_EOL;
			print_r($o->objTest_ByRef);

Title("** String Offset Tests **");
		
	Title("String Offsets:");
		echo '$o->str_ByRef = "string".PHP_EOL;'.PHP_EOL;
			$o->str_ByRef = "string".PHP_EOL;		
		echo 'print_r($o->str_ByRef);'.PHP_EOL;
			print_r($o->str_ByRef);
		echo '$x = 3;'.PHP_EOL;
			$x = 3;
		echo '$o->str_ByRef{$x} = "bar";'.PHP_EOL;
			$o->str_ByRef{$x} = "bar";
		echo 'print_r($o->str_ByRef);'.PHP_EOL;
			print_r($o->str_ByRef);
?>
===DONE===
--EXPECTF--
** Array Reference Tests **
---------------------------


Attempt to append to a non-by-reference array accessor:
-------------------------------------------------------
Array
(
)
$o->tArray[] = 1

Notice: Indirect modification of overloaded property TimePeriod::$tArray has no effect in %s on line %d
Array
(
)


Append to an array-by-ref:
--------------------------
$o->tArray_ByRef = array('initial set');
print_r($o->tArray_ByRef);
Array
(
    [0] => initial set
)
$o->tArray_ByRef[] = 1
Array
(
    [0] => initial set
    [1] => 1
)


Direct set to an array-by-ref:
------------------------------
$o->tArray_ByRef = array('initial set 2');
print_r($o->tArray_ByRef);
Array
(
    [0] => initial set 2
)
$o->tArray_ByRef[] = 1
Array
(
    [0] => initial set 2
    [123] => 4
)


Copy reference to an array, modify reference copy, check original:
------------------------------------------------------------------
$o->tArray_ByRef = array('initial set 3');
print_r($o->tArray_ByRef);
Array
(
    [0] => initial set 3
)
$foobar = &$o->tArray_ByRef;
print_r($foobar);
Array
(
    [0] => initial set 3
)
$foobar[123] = 6;
print_r($foobar);
Array
(
    [0] => initial set 3
    [123] => 6
)
print_r($o->tArray_ByRef);
Array
(
    [0] => initial set 3
    [123] => 6
)


** Object Tests **
------------------


Set property of sub-object:
---------------------------
$o->objTest_ByRef = new stdClass();
print_r($o->objTest_ByRef);
stdClass Object
(
)
$o->objTest_ByRef->baz = 1;
print_r($o->objTest_ByRef);
stdClass Object
(
    [baz] => 1
)


Set property of empty property as though it were an object:
-----------------------------------------------------------
unset($o->objTest_ByRef);
print_r($o->objTest_ByRef);
$o->objTest_ByRef->baz = 1;

Warning: Creating default object from empty value in %s on line %d
print_r($o->objTest_ByRef);
stdClass Object
(
    [baz] => 1
)


Set property of string property as though it were an object:
------------------------------------------------------------
$o->objTest_ByRef = "string".PHP_EOL;
print_r($o->objTest_ByRef);
string
$o->objTest_ByRef->baz = 1;

Warning: Attempt to assign property of non-object in %s on line %d
print_r($o->objTest_ByRef);
string


** String Offset Tests **
-------------------------


String Offsets:
---------------
$o->str_ByRef = "string".PHP_EOL;
print_r($o->str_ByRef);
string
$x = 3;
$o->str_ByRef{$x} = "bar";
print_r($o->str_ByRef);
strbng
===DONE===