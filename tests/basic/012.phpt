--TEST--
Testing $argc and $argv handling (cli)
--INI--
register_argc_argv=1
variables_order=GPS
--ARGS--
ab cd ef 123 test
--FILE--
<?php
$argc = $_SERVER['argc'];
$argv = $_SERVER['argv'];

for ($i=1; $i<$argc; $i++) {
    echo ($i-1).": ".$argv[$i]."\n";
}

?>
--EXPECT--
0: ab
1: cd
2: ef
3: 123
4: test
