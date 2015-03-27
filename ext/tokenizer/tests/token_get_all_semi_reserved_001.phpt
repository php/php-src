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
L7: T_CLASS class
;
Done
