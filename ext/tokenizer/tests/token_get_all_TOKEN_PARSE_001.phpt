--TEST--
Semi reserved words support: member access
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$tokens = token_get_all('<?php
X::continue;
X::$continue;
$x->$continue;
X::continue();
$x->continue();
X::class;
$x->__halt_compiler();

class X {
    const CONTINUE = 1;
    public $x = self::CONTINUE + 1;
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
--EXPECTF--
L1: T_OPEN_TAG <?php

L2: T_STRING X
L2: T_DOUBLE_COLON ::
L2: T_STRING continue
;
L3: T_STRING X
L3: T_DOUBLE_COLON ::
L3: T_VARIABLE $continue
;
L4: T_VARIABLE $x
L4: T_OBJECT_OPERATOR ->
L4: T_VARIABLE $continue
;
L5: T_STRING X
L5: T_DOUBLE_COLON ::
L5: T_STRING continue
(
)
;
L6: T_VARIABLE $x
L6: T_OBJECT_OPERATOR ->
L6: T_STRING continue
(
)
;
L7: T_STRING X
L7: T_DOUBLE_COLON ::
L7: T_STRING class
;
L8: T_VARIABLE $x
L8: T_OBJECT_OPERATOR ->
L8: T_STRING __halt_compiler
(
)
;
L10: T_CLASS class
L10: T_STRING X
{
L11: T_CONST const
L11: T_STRING CONTINUE
=
L11: T_LNUMBER 1
;
L12: T_PUBLIC public
L12: T_VARIABLE $x
=
L12: T_STRING self
L12: T_DOUBLE_COLON ::
L12: T_STRING CONTINUE
+
L12: T_LNUMBER 1
;
}
Done