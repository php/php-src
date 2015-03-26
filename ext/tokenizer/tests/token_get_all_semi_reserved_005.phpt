--TEST--
Semi reserved words support: trait adaptations
--FILE--
<?php
$tokens = token_get_all('<?php
    use N\S{ ClassA, ClassB };

    class SomeClass {
        use A, B {
            B::insteadof insteadof A;
            A::as as as;
            as as as;
            as as private as;
            try as private /** */ while;
        }
    }
  }
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

L2: T_USE use
L2: T_STRING N
L2: T_NS_SEPARATOR \
L2: T_STRING S
{
L2: T_STRING ClassA
,
L2: T_STRING ClassB
}
;
L4: T_CLASS class
L4: T_STRING SomeClass
{
L5: T_USE use
L5: T_STRING A
,
L5: T_STRING B
{
L6: T_STRING B
L6: T_DOUBLE_COLON ::
L6: T_STRING insteadof
L6: T_INSTEADOF insteadof
L6: T_STRING A
;
L7: T_STRING A
L7: T_DOUBLE_COLON ::
L7: T_STRING as
L7: T_AS as
L7: T_STRING as
;
L8: T_STRING as
L8: T_AS as
L8: T_STRING as
;
L9: T_STRING as
L9: T_AS as
L9: T_PRIVATE private
L9: T_STRING as
;
L10: T_STRING try
L10: T_AS as
L10: T_PRIVATE private
L10: T_DOC_COMMENT /** */
L10: T_STRING while
;
}
}
}
Done
