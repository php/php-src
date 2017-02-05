--TEST--
Test hrtime() returns nanoseconds
--FILE--
<?php

$time = hrtime();
usleep(500000);
$diff = hrtime() - $time;

if ($diff < 500000000) {
    print "fail";
} else if ($diff > 1000000000) {
    print "fail";
} else {
    print "OK";
}

?>
--EXPECT--
OK
