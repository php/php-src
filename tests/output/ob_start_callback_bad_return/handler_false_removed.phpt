--TEST--
ob_start(): Check behaviour with deprecation when OOM triggers handler removal (handler returns false)
--INI--
memory_limit=2M
--FILE--
<?php

ob_start(function() {
    // We are out of memory, now trigger a deprecation
    return false;
});

$a = [];
// trigger OOM in a resize operation
while (1) {
    $a[] = 1;
}

?>
--EXPECTF--
Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
