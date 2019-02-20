--TEST--
Pass uninitialised objects and arrays by reference to test implicit initialisation.
--FILE--
<?php

function refs(&$ref1, &$ref2, &$ref3, &$ref4, &$ref5) {
  $ref1 = "Ref1 changed";
  $ref2 = "Ref2 changed";
  $ref3 = "Ref3 changed";
  $ref4 = "Ref4 changed";
  $ref5 = "Ref5 changed";
}


class C {

	function __construct(&$ref1, &$ref2, &$ref3, &$ref4, &$ref5) {
	  $ref1 = "Ref1 changed";
	  $ref2 = "Ref2 changed";
	  $ref3 = "Ref3 changed";
	  $ref4 = "Ref4 changed";
	  $ref5 = "Ref5 changed";
	}

	function refs(&$ref1, &$ref2, &$ref3, &$ref4, &$ref5) {
	  $ref1 = "Ref1 changed";
	  $ref2 = "Ref2 changed";
	  $ref3 = "Ref3 changed";
	  $ref4 = "Ref4 changed";
	  $ref5 = "Ref5 changed";
	}

	static function static_refs(&$ref1, &$ref2, &$ref3, &$ref4, &$ref5) {
	  $ref1 = "Ref1 changed";
	  $ref2 = "Ref2 changed";
	  $ref3 = "Ref3 changed";
	  $ref4 = "Ref4 changed";
	  $ref5 = "Ref5 changed";
	}

}

echo "\n ---- Pass uninitialised array & object by ref: function call ---\n";
unset($u1, $u2, $u3, $u4, $u5);
refs($u1[0], $u2[0][1], $u3->a, $u4->a->b, $u5->a->b->c);
var_dump($u1, $u2, $u3, $u4, $u5);

echo "\n ---- Pass uninitialised arrays & objects by ref: static method call ---\n";
unset($u1, $u2, $u3, $u4, $u5);
C::static_refs($u1[0], $u2[0][1], $u3->a, $u4->a->b, $u5->a->b->c);
var_dump($u1, $u2, $u3, $u4, $u5);

echo "\n\n---- Pass uninitialised arrays & objects by ref: constructor ---\n";
unset($u1, $u2, $u3, $u4, $u5);
$c = new C($u1[0], $u2[0][1], $u3->a, $u4->a->b, $u5->a->b->c);
var_dump($u1, $u2, $u3, $u4, $u5);

echo "\n ---- Pass uninitialised arrays & objects by ref: instance method call ---\n";
unset($u1, $u2, $u3, $u4, $u5);
$c->refs($u1[0], $u2[0][1], $u3->a, $u4->a->b, $u5->a->b->c);
var_dump($u1, $u2, $u3, $u4, $u5);

?>
--EXPECTF--
---- Pass uninitialised array & object by ref: function call ---

Warning: Attempt to modify property 'a' of non-object in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to modify property 'b' of non-object in %s on line %d

Notice: Undefined variable: u5 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Notice: Trying to get property 'b' of non-object in %s on line %d

Warning: Attempt to modify property 'c' of non-object in %s on line %d

Notice: Undefined variable: u3 in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Undefined variable: u5 in %s on line %d
array(1) {
  [0]=>
  string(12) "Ref1 changed"
}
array(1) {
  [0]=>
  array(1) {
    [1]=>
    string(12) "Ref2 changed"
  }
}
NULL
NULL
NULL

 ---- Pass uninitialised arrays & objects by ref: static method call ---

Warning: Attempt to modify property 'a' of non-object in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to modify property 'b' of non-object in %s on line %d

Notice: Undefined variable: u5 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Notice: Trying to get property 'b' of non-object in %s on line %d

Warning: Attempt to modify property 'c' of non-object in %s on line %d

Notice: Undefined variable: u3 in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Undefined variable: u5 in %s on line %d
array(1) {
  [0]=>
  string(12) "Ref1 changed"
}
array(1) {
  [0]=>
  array(1) {
    [1]=>
    string(12) "Ref2 changed"
  }
}
NULL
NULL
NULL


---- Pass uninitialised arrays & objects by ref: constructor ---

Warning: Attempt to modify property 'a' of non-object in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to modify property 'b' of non-object in %s on line %d

Notice: Undefined variable: u5 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Notice: Trying to get property 'b' of non-object in %s on line %d

Warning: Attempt to modify property 'c' of non-object in %s on line %d

Notice: Undefined variable: u3 in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Undefined variable: u5 in %s on line %d
array(1) {
  [0]=>
  string(12) "Ref1 changed"
}
array(1) {
  [0]=>
  array(1) {
    [1]=>
    string(12) "Ref2 changed"
  }
}
NULL
NULL
NULL

 ---- Pass uninitialised arrays & objects by ref: instance method call ---

Warning: Attempt to modify property 'a' of non-object in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to modify property 'b' of non-object in %s on line %d

Notice: Undefined variable: u5 in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Notice: Trying to get property 'b' of non-object in %s on line %d

Warning: Attempt to modify property 'c' of non-object in %s on line %d

Notice: Undefined variable: u3 in %s on line %d

Notice: Undefined variable: u4 in %s on line %d

Notice: Undefined variable: u5 in %s on line %d
array(1) {
  [0]=>
  string(12) "Ref1 changed"
}
array(1) {
  [0]=>
  array(1) {
    [1]=>
    string(12) "Ref2 changed"
  }
}
NULL
NULL
NULL
