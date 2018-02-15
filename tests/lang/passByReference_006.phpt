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

}

echo "\n ---- Pass uninitialised array & object by ref: function call ---\n";
unset($u1, $u2, $u3, $u4, $u5);
refs($u1[0], $u2[0][1], $u3->a, $u4->a->b, $u5->a->b->c);
var_dump($u1, $u2, $u3, $u4, $u5);

echo "\n ---- Pass uninitialised arrays & objects by ref: static method call ---\n";
unset($u1, $u2, $u3, $u4, $u5);
C::refs($u1[0], $u2[0][1], $u3->a, $u4->a->b, $u5->a->b->c);
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

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d
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
object(stdClass)#%d (1) {
  ["a"]=>
  string(12) "Ref3 changed"
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    string(12) "Ref4 changed"
  }
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    object(stdClass)#%d (1) {
      ["c"]=>
      string(12) "Ref5 changed"
    }
  }
}

 ---- Pass uninitialised arrays & objects by ref: static method call ---

Deprecated: Non-static method C::refs() should not be called statically in %s on line 39

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d
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
object(stdClass)#%d (1) {
  ["a"]=>
  string(12) "Ref3 changed"
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    string(12) "Ref4 changed"
  }
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    object(stdClass)#%d (1) {
      ["c"]=>
      string(12) "Ref5 changed"
    }
  }
}


---- Pass uninitialised arrays & objects by ref: constructor ---

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d
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
object(stdClass)#%d (1) {
  ["a"]=>
  string(12) "Ref3 changed"
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    string(12) "Ref4 changed"
  }
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    object(stdClass)#%d (1) {
      ["c"]=>
      string(12) "Ref5 changed"
    }
  }
}

 ---- Pass uninitialised arrays & objects by ref: instance method call ---

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d

Warning: Creating default object from empty value in %spassByReference_006.php on line %d
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
object(stdClass)#%d (1) {
  ["a"]=>
  string(12) "Ref3 changed"
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    string(12) "Ref4 changed"
  }
}
object(stdClass)#%d (1) {
  ["a"]=>
  object(stdClass)#%d (1) {
    ["b"]=>
    object(stdClass)#%d (1) {
      ["c"]=>
      string(12) "Ref5 changed"
    }
  }
}
