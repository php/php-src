--TEST--
getopt#006 (optind #1)
--ARGS--
-a 1 -b 2 test
--INI--
register_argc_argv=On
variables_order=GPS
--FILE--
<?php
    $optind = null;
    getopt('a:b:', [], $optind);
    var_dump($optind);
?>
--EXPECT--
int(5)
