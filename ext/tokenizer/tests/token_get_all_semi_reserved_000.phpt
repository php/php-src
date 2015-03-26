--TEST--
Semi reserved words support: general test
--FILE--
<?php

$tokens = token_get_all('<?php

  use Some\NS{ SomeClass };

  class SomeClass {

      const CONST = 1, YIELD = [1, [2, FOO::TRAIT]], EXP = self::CONST + 1, CONTINUE = ",";

      use A, B {
          B::insteadof insteadof A;
          A::as as as;
          as as as;
          as as private as;
          try as private /** */ while;
      }

      function /*comment*/ function() {
          function function(){}
          return function() {}
          return function() use ($var) {
              return X::protected;
                     // ^ invalid, should be T_PROTECTED not T_STRING
          }
      }

      function function() {}
  }

  $obj->function();
  $obj::function();
  SomeClass::function();

  function function(){ return "invalid"; }
  const CONST = "invalid";
');

array_walk($tokens, function($tk) {
  if(is_array($tk)) {
    if(($t = token_name($tk[0])) == 'T_WHITESPACE') return;
    echo "L{$tk[2]}: ".$t." {$tk[1]}", PHP_EOL;
  }
  else echo $tk, PHP_EOL;
});

echo "Done";

?>
--EXPECTF--
L1: T_OPEN_TAG <?php

L3: T_USE use
L3: T_STRING Some
L3: T_NS_SEPARATOR \
L3: T_STRING NS
{
L3: T_STRING SomeClass
}
;
L5: T_CLASS class
L5: T_STRING SomeClass
{
L7: T_CONST const
L7: T_STRING CONST
=
L7: T_LNUMBER 1
,
L7: T_STRING YIELD
=
[
L7: T_LNUMBER 1
,
[
L7: T_LNUMBER 2
,
L7: T_STRING FOO
L7: T_DOUBLE_COLON ::
L7: T_STRING TRAIT
]
]
,
L7: T_STRING EXP
=
L7: T_STRING self
L7: T_DOUBLE_COLON ::
L7: T_STRING CONST
+
L7: T_LNUMBER 1
,
L7: T_STRING CONTINUE
=
L7: T_CONSTANT_ENCAPSED_STRING ","
;
L9: T_USE use
L9: T_STRING A
,
L9: T_STRING B
{
L10: T_STRING B
L10: T_DOUBLE_COLON ::
L10: T_STRING insteadof
L10: T_INSTEADOF insteadof
L10: T_STRING A
;
L11: T_STRING A
L11: T_DOUBLE_COLON ::
L11: T_STRING as
L11: T_AS as
L11: T_STRING as
;
L12: T_STRING as
L12: T_AS as
L12: T_STRING as
;
L13: T_STRING as
L13: T_AS as
L13: T_PRIVATE private
L13: T_STRING as
;
L14: T_STRING try
L14: T_AS as
L14: T_PRIVATE private
L14: T_DOC_COMMENT /** */
L14: T_STRING while
;
}
L17: T_FUNCTION function
L17: T_COMMENT /*comment*/
L17: T_STRING function
(
)
{
L18: T_FUNCTION function
L18: T_FUNCTION function
(
)
{
}
L19: T_RETURN return
L19: T_FUNCTION function
(
)
{
}
L20: T_RETURN return
L20: T_FUNCTION function
(
)
L20: T_USE use
(
L20: T_VARIABLE $var
)
{
L21: T_RETURN return
L21: T_STRING X
L21: T_DOUBLE_COLON ::
L21: T_PROTECTED protected
;
L22: T_COMMENT // ^ invalid, should be T_PROTECTED not T_STRING

}
}
L26: T_FUNCTION function
L26: T_STRING function
(
)
{
}
}
L29: T_VARIABLE $obj
L29: T_OBJECT_OPERATOR ->
L29: T_STRING function
(
)
;
L30: T_VARIABLE $obj
L30: T_DOUBLE_COLON ::
L30: T_STRING function
(
)
;
L31: T_STRING SomeClass
L31: T_DOUBLE_COLON ::
L31: T_STRING function
(
)
;
L33: T_FUNCTION function
L33: T_FUNCTION function
(
)
{
L33: T_RETURN return
L33: T_CONSTANT_ENCAPSED_STRING "invalid"
;
}
L34: T_CONST const
L34: T_CONST CONST
=
L34: T_CONSTANT_ENCAPSED_STRING "invalid"
;
Done
