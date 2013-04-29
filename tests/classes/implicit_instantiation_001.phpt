--TEST--
Implicit object instantiation when accessing properties of non-object.
--FILE--
<?php
class C {
	// These values get implicitly converted to objects
	public $boolFalse = false;
	public $emptyString = '';
	public $null = null;

	// These values do not get implicitly converted to objects
	public $boolTrue = true;
	public $nonEmptyString = 'hello';
	public $intZero = 0;
}

$c = new C;
foreach($c as $name => $value) {
	echo "\n\n---( \$c->$name )---";
	echo "\n  --> Attempting implicit conversion to object using increment...\n";
	$c->$name->prop++;
	$c->$name = $value; // reset value in case implicit conversion was successful
	
	echo "\n  --> Attempting implicit conversion to object using assignment...\n";
	$c->$name->prop = "Implicit instantiation!";
	$c->$name = $value; // reset value in case implicit conversion was successful

	echo "\n  --> Attempting implicit conversion to object using combined assignment...\n";
	$c->$name->prop .= " Implicit instantiation!";
}

echo "\n\n\n --> Resulting object:";
var_dump($c);

?>
--EXPECTF--


---( $c->boolFalse )---
  --> Attempting implicit conversion to object using increment...

Warning: Creating default object from empty value in %s on line 18

  --> Attempting implicit conversion to object using assignment...

Warning: Creating default object from empty value in %s on line 22

  --> Attempting implicit conversion to object using combined assignment...

Warning: Creating default object from empty value in %s on line 26


---( $c->emptyString )---
  --> Attempting implicit conversion to object using increment...

Warning: Creating default object from empty value in %s on line 18

  --> Attempting implicit conversion to object using assignment...

Warning: Creating default object from empty value in %s on line 22

  --> Attempting implicit conversion to object using combined assignment...

Warning: Creating default object from empty value in %s on line 26


---( $c->null )---
  --> Attempting implicit conversion to object using increment...

Warning: Creating default object from empty value in %s on line 18

  --> Attempting implicit conversion to object using assignment...

Warning: Creating default object from empty value in %s on line 22

  --> Attempting implicit conversion to object using combined assignment...

Warning: Creating default object from empty value in %s on line 26


---( $c->boolTrue )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to %s property of non-object in %s on line 18

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property of non-object in %s on line 22

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property of non-object in %s on line 26


---( $c->nonEmptyString )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to %s property of non-object in %s on line 18

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property of non-object in %s on line 22

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property of non-object in %s on line 26


---( $c->intZero )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to %s property of non-object in %s on line 18

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property of non-object in %s on line 22

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property of non-object in %s on line 26



 --> Resulting object:object(C)#%d (6) {
  [%u|b%"boolFalse"]=>
  object(stdClass)#%d (1) {
    [%u|b%"prop"]=>
    %unicode|string%(24) " Implicit instantiation!"
  }
  [%u|b%"emptyString"]=>
  object(stdClass)#%d (1) {
    [%u|b%"prop"]=>
    %unicode|string%(24) " Implicit instantiation!"
  }
  [%u|b%"null"]=>
  object(stdClass)#%d (1) {
    [%u|b%"prop"]=>
    %unicode|string%(24) " Implicit instantiation!"
  }
  [%u|b%"boolTrue"]=>
  bool(true)
  [%u|b%"nonEmptyString"]=>
  %unicode|string%(5) "hello"
  [%u|b%"intZero"]=>
  int(0)
}