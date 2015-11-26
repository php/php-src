--TEST--
try { yield } finally { return }
--FILE--
<?php
function foo($f, $t) {
    for ($i = $f; $i <= $t; $i++) {
        try {
            yield $i;
        } finally {
            return;
        }
    }
}
foreach (foo(1, 5) as $x) {
    echo $x, "\n";
}
--EXPECT--
1
