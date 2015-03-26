--TEST--
Semi reserved words support: class const lists
--FILE--
<?php
$tokens = token_get_all('<?php
  class SomeClass {
      const NOT_LIST = 1;
      const CONST = 1,
            CONTINUE = self::CONST + 1,
            ARRAY = [self::CONST, 2 => [3, 4], 5],
            CLASS = 0;
  }

  const CONST = 1,
        CONTINUE = CONST + 1,
        ARRAY = [1, 2 => [3, 4], 5],
        CLASS = 0;
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

L2: T_CLASS class
L2: T_STRING SomeClass
{
L3: T_CONST const
L3: T_STRING NOT_LIST
=
L3: T_LNUMBER 1
;
L4: T_CONST const
L4: T_STRING CONST
=
L4: T_LNUMBER 1
,
L5: T_STRING CONTINUE
=
L5: T_STRING self
L5: T_DOUBLE_COLON ::
L5: T_STRING CONST
+
L5: T_LNUMBER 1
,
L6: T_STRING ARRAY
=
[
L6: T_STRING self
L6: T_DOUBLE_COLON ::
L6: T_STRING CONST
,
L6: T_LNUMBER 2
L6: T_DOUBLE_ARROW =>
[
L6: T_LNUMBER 3
,
L6: T_LNUMBER 4
]
,
L6: T_LNUMBER 5
]
,
L7: T_CLASS CLASS
=
L7: T_LNUMBER 0
;
}
L10: T_CONST const
L10: T_CONST CONST
=
L10: T_LNUMBER 1
,
L11: T_CONTINUE CONTINUE
=
L11: T_CONST CONST
+
L11: T_LNUMBER 1
,
L12: T_ARRAY ARRAY
=
[
L12: T_LNUMBER 1
,
L12: T_LNUMBER 2
L12: T_DOUBLE_ARROW =>
[
L12: T_LNUMBER 3
,
L12: T_LNUMBER 4
]
,
L12: T_LNUMBER 5
]
,
L13: T_CLASS CLASS
=
L13: T_LNUMBER 0
;
Done
