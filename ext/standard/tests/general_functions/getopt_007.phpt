--TEST--
getopt#007 (optind #2)
--ARGS--
-a 1 -b 2 -- -c nope test
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
int(6)
