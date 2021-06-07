--TEST--
debug_backtrace skip_last into generator place holder frames
--FILE--
<?php
function Generate() {
    yield from [debug_backtrace(skip_last: 0), 
                debug_backtrace(skip_last: 1), 
                debug_backtrace(skip_last: 2)];
}

foreach(Generate() as $trace) {
    if (!$trace) {
        printf("empty\n\n");
        continue;
    }
    
    foreach ($trace as $frame) {
        printf("%s in %s on line %d\n", $frame["function"], $frame["file"], $frame["line"]);
    }
    echo PHP_EOL;
}
?>
--EXPECTF--
debug_backtrace in %s on line 3
Generate in %s on line 8

Generate in %s on line 8

empty

