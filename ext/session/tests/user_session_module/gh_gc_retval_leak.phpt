--TEST--
session_gc(): user handler returning non-bool/non-int does not leak memory
--INI--
session.gc_probability=0
session.save_handler=files
--EXTENSIONS--
session
--FILE--
<?php
ob_start();

// Procedural API has no return type enforcement, so gc can return a string
// (reference-counted), which PS_GC_FUNC(user) previously did not free.
session_set_save_handler(
    function(string $path, string $name) { return true; },
    function() { return true; },
    function(string $id): string|false { return ""; },
    function(string $id, string $data) { return true; },
    function(string $id) { return true; },
    function(int $max) { return str_repeat("x", random_int(100, 100)); }
);

session_start();
$result = session_gc();
var_dump($result);
session_write_close();

ob_end_flush();
?>
--EXPECTF--

Deprecated: session_set_save_handler(): Providing individual callbacks instead of an object implementing SessionHandlerInterface is deprecated in %s on line %d
bool(false)
