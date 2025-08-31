--TEST--
Semi reserved words support: class const
--EXTENSIONS--
tokenizer
--FILE--
<?php
$tokens = token_get_all('<?php
  class SomeClass {
      const CONST = 1;
      const CONTINUE = (self::CONST + 1);
      const ARRAY = [1, self::CONTINUE => [3, 4], 5];
  }
', TOKEN_PARSE);

array_walk($tokens, function($tk) {
  if(is_array($tk)) {
    if(($t = token_name($tk[0])) == 'T_WHITESPACE') return;
    echo "L{$tk[2]}: ".$t." {$tk[1]}", PHP_EOL;
  }
  else echo $tk, PHP_EOL;
});

echo "Done";

?>
--EXPECT--
L1: T_OPEN_TAG <?php

L2: T_CLASS class
L2: T_STRING SomeClass
{
L3: T_CONST const
L3: T_STRING CONST
=
L3: T_LNUMBER 1
;
L4: T_CONST const
L4: T_STRING CONTINUE
=
(
L4: T_STRING self
L4: T_DOUBLE_COLON ::
L4: T_STRING CONST
+
L4: T_LNUMBER 1
)
;
L5: T_CONST const
L5: T_STRING ARRAY
=
[
L5: T_LNUMBER 1
,
L5: T_STRING self
L5: T_DOUBLE_COLON ::
L5: T_STRING CONTINUE
L5: T_DOUBLE_ARROW =>
[
L5: T_LNUMBER 3
,
L5: T_LNUMBER 4
]
,
L5: T_LNUMBER 5
]
;
}
Done
