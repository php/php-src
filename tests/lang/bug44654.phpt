--TEST--
Bug #44654 (syntax error for #)
--FILE--
#<?php echo 1; ?>
<?php if (1) { ?>#<?php } ?>
#<?php echo 1; ?>
--EXPECT--
#1##1
