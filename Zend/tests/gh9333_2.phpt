--TEST--
GH-9333: Catchable compile error shouldn't leak memory
--FILE--
<?php

for ($i = 1; $i <= 1000; $i++) {
    // First iterations may allocate unrelated memory
    if ($i === 10) {
        $start = memory_get_usage();
    }

    try {
        eval('class Invalid { function foo(); }');
    } catch (Throwable) {}
}
var_dump(memory_get_usage() - $start);
?>
--EXPECT--
int(0)
