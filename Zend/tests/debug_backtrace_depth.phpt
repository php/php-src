--TEST--
debug_backtrace_depth
--FILE--
<?php

function example(int $i, int $max) {
    printf("i=%d depth=%d depth(limit=5)=%d\n", $i, debug_backtrace_depth(), debug_backtrace_depth(5));
    if ($i < $max) {
        example($i + 1, $max);
    }
}
example(0, 7);
printf("at entry point depth=%d depth(limit=5)=%d\n", debug_backtrace_depth(), debug_backtrace_depth(5));
?>
--EXPECT--
i=0 depth=1 depth(limit=5)=1
i=1 depth=2 depth(limit=5)=2
i=2 depth=3 depth(limit=5)=3
i=3 depth=4 depth(limit=5)=4
i=4 depth=5 depth(limit=5)=5
i=5 depth=6 depth(limit=5)=5
i=6 depth=7 depth(limit=5)=5
i=7 depth=8 depth(limit=5)=5
at entry point depth=0 depth(limit=5)=0