--TEST--
Testing $argc and $argv handling (GET)
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
--EXPECT--
0: ab
1: cd
2: ef
3: 123
4: test
