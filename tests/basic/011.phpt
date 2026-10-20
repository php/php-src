--TEST--
Testing $argc and $argv handling (GET)
--SKIPIF--
<?php
    if(substr(PHP_OS, 0, 3) == 'WIN') die("skip on windows: --INI-- is ignored due to 4b9cd27ff5c0177dcb160caeae1ea79e761ada58");
?>
--INI--
register_argc_argv=1
--GET--
ab+cd+ef+123+test
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
0: ab
1: cd
2: ef
3: 123
4: test
