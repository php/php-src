--TEST--
#[\NoDiscard]: Casting to (void) suppresses (Class).
--FILE--
<?php

#[\NoDiscard]
class Test { }
function Test() { return new (__FUNCTION__)(); }

#[\NoDiscard("this is important")]
class Test2 { }
function Test2() { return new (__FUNCTION__)(); }

(void)Test();
(void)Test2();

?>
DONE
--EXPECT--
DONE
