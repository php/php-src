--TEST--
Testing $argc and $argv handling (GET)
--INI--
register_argc_argv=1
--GET--
foo=ab+cd+ef+123+test
--FILE--
<?php
$argc = $_SERVER['argc'];
$argv = $_SERVER['argv'];

for ($i=0; $i<$argc; $i++) {
    echo "$i: ".$argv[$i]."\n";
}

?>
--EXPECTF--
Deprecated: Deriving $_SERVER['argv'] from the query string is deprecated. Configure register_argc_argv=0 to turn this message off in %s on line %d
0: foo=ab
1: cd
2: ef
3: 123
4: test
