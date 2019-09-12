--TEST--
Bug #78531 (Crash when using undefined variable as object)
--FILE--
<?php
@$u1->a += 5;
var_dump($u1->a);
@$x = ++$u2->a;
var_dump($u2->a);
@$x = $u3->a++;
var_dump($u3->a);
@$u4->a->a += 5;
var_dump($u4->a->a);
?>
--EXPECT--
int(5)
int(1)
int(1)
int(5)