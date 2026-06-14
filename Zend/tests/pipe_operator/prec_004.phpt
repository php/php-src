--TEST--
Pipe precedence 004
--FILE--
<?php

null
    |> (fn() => print (new Exception)->getTraceAsString() . "\n\n")
    |> (fn() => print (new Exception)->getTraceAsString() . "\n\n");

?>
--EXPECTF--
#0 %s(%d): {closure:%s:%d}(NULL)
#1 {main}

#0 %s(%d): {closure:%s:%d}(1)
#1 {main}
