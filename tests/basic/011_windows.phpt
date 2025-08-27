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
Deprecated: Deriving $_SERVER['argc'] and $_SERVER['argv'] from $_SERVER['QUERY_STRING'] is deprecated, configure register_argc_argv=0 to suppress this message and access the query parameters via $_SERVER['QUERY_STRING'] or $_GET in %s on line %d
0: foo=ab
1: cd
2: ef
3: 123
4: test
