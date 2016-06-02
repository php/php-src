--TEST--
test finish and leave commands
--PHPDBG--
b bar
b 5
r
finish
leave
leave
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at bar]
prompt> [Breakpoint #1 added at %s:5]
prompt> [Breakpoint #0 in bar() at %s:9, hits: 1]
>00009:     return "world";
 00010: }
 00011: 
prompt> [Breakpoint #1 at %s:5, hits: 1]
>00005:     return ["hello", $other];
 00006: }
 00007: 
prompt> [Breaking for leave at %s:5]
>00005:     return ["hello", $other];
 00006: }
 00007: 
prompt> [Already at the end of the function]
prompt> 
--FILE--
<?php
function foo() {
    $other = bar();
    
    return ["hello", $other];
}

function bar() {
    return "world";
}

foo();
