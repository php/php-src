--TEST--
Implicit object instantiation when accessing properties of non-object (no longer supported)
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

Warning: Attempt to increment/decrement property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d


---( $c->emptyString )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to increment/decrement property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d


---( $c->null )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to increment/decrement property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d


---( $c->boolTrue )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to increment/decrement property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d


---( $c->nonEmptyString )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to increment/decrement property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d


---( $c->intZero )---
  --> Attempting implicit conversion to object using increment...

Warning: Attempt to increment/decrement property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d

  --> Attempting implicit conversion to object using combined assignment...

Warning: Attempt to assign property 'prop' of non-object in %s on line %d



 --> Resulting object:object(C)#1 (6) {
  ["boolFalse"]=>
  bool(false)
  ["emptyString"]=>
  string(0) ""
  ["null"]=>
  NULL
  ["boolTrue"]=>
  bool(true)
  ["nonEmptyString"]=>
  string(5) "hello"
  ["intZero"]=>
  int(0)
}
