--TEST--
Bug #69740: finally in generator (yield) swallows exception in iteration
--FILE--
<?php

function generate() {
    try {
        yield 1;
        yield 2;
    } finally {
        echo "finally\n";
    }
}

foreach (generate() as $i) {
    echo $i, "\n";
    throw new Exception();
}

?>
--EXPECTF--
1
finally

Fatal error: Uncaught exception 'Exception' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
