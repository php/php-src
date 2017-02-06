--TEST--
Test hrtime() aligns with microtime()
--FILE--
<?php

$hrtime = hrtime();
$microtime = microtime(1);

usleep(500000);

$hrdiff = hrtime() - $hrtime;
$microdiff = microtime(1) - $microtime;

if (abs($hrdiff - $microdiff) > 0.0001) {
    print "fail";
} else {
    print "OK";
}

?>
--EXPECT--
OK
