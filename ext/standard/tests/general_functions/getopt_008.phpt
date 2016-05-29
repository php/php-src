--TEST--
getopt#008 (optind #3)
--ARGS--
-a 1 -b 2
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
