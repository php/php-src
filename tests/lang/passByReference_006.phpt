--TEST--
Pass uninitialised objects and arrays by reference to test implicit initialisation.
--FILE--
<?php

function refs(&$ref1, &$ref2) {
  $ref1 = "Ref1 changed";
  $ref2 = "Ref2 changed";
}


class C {

    function __construct(&$ref1, &$ref2) {
      $ref1 = "Ref1 changed";
      $ref2 = "Ref2 changed";
    }

    function refs(&$ref1, &$ref2) {
      $ref1 = "Ref1 changed";
      $ref2 = "Ref2 changed";
    }

    static function static_refs(&$ref1, &$ref2) {
      $ref1 = "Ref1 changed";
      $ref2 = "Ref2 changed";
    }

}

echo "\n ---- Pass uninitialised array & object by ref: function call ---\n";
unset($u1, $u2);
refs($u1[0], $u2[0][1]);
var_dump($u1, $u2);

echo "\n ---- Pass uninitialised arrays & objects by ref: static method call ---\n";
unset($u1, $u2);
C::static_refs($u1[0], $u2[0][1]);
var_dump($u1, $u2);

echo "\n\n---- Pass uninitialised arrays & objects by ref: constructor ---\n";
unset($u1, $u2);
$c = new C($u1[0], $u2[0][1]);
var_dump($u1, $u2);

echo "\n ---- Pass uninitialised arrays & objects by ref: instance method call ---\n";
unset($u1, $u2);
$c->refs($u1[0], $u2[0][1]);
var_dump($u1, $u2);

?>
--EXPECT--
---- Pass uninitialised array & object by ref: function call ---
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

 ---- Pass uninitialised arrays & objects by ref: static method call ---
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


---- Pass uninitialised arrays & objects by ref: constructor ---
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

 ---- Pass uninitialised arrays & objects by ref: instance method call ---
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
