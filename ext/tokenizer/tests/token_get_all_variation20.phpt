--TEST--
T_CAST
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

$phpstr = '<?php (int) "1"; ?>';

$tokens = token_get_all($phpstr);

array_walk($tokens, function($tk) {
  if(is_array($tk)) {
    if(($t = token_name($tk[0])) == 'T_WHITESPACE') return;
    echo "L{$tk[2]}: ".$t." {$tk[1]}", PHP_EOL;
  }
  else echo $tk, PHP_EOL;
});

echo PHP_EOL, 'Done';
?>
--EXPECT--
L1: T_OPEN_TAG <?php 
L1: T_CAST (int)
L1: T_CONSTANT_ENCAPSED_STRING "1"
;
L1: T_CLOSE_TAG ?>

Done
