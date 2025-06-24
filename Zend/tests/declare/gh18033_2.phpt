--TEST--
GH-18033 (NULL-ptr dereference when using register_tick_function in ob_start)
--DESCRIPTION--
Needs --repeat 2 or something similar to reproduce
--CREDITS--
clesmian
--FILE--
<?php
ob_start(function() {
    declare(ticks=1);
    register_tick_function(
       function() { }
    );
});
?>
--EXPECTF--
Deprecated: PHP Request Shutdown: Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d
