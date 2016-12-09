--TEST--
Use conflicts should not occur across eval()s
--FILE--
<?php

/* It is important that these two eval()s occur on the same line,
 * as this forces them to have the same filename. */
eval("class A {}"); eval("use Foo\A;");

?>
===DONE===
--EXPECT--
===DONE===
