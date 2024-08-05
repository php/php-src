--TEST--
GH-15208 (phpdbg segfault on empty class/function names)
--PHPDBG--
r
c
--FILE--
<?php

function test($function) {
    if (str_contains($function, "zend")) {
        return;
    }
    ob_start();
    try {
        @$function();
    } catch (Throwable) {
    }
    try {
        @$function(null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null);
    } catch (Throwable) {
    }
    ob_end_clean();
}

foreach (get_defined_functions()["internal"] as $function) {
    test($function);
}
?>
--EXPECTF--
[Successful compilation of %s]
%a
Notice: ob_end_clean(): Failed to delete buffer. No buffer to delete in %s on line %d
%A
%a
>00009:         @$function();
 00010:     } catch (Throwable) {
 00011:     }
prompt> [Cannot set breakpoint in %s, it is not a regular file]
[Breakpoint #1 added at ::]
[Breakpoint #2 added at ]
[Script ended normally]
 [The stack contains nothing !]
