--TEST--
Testing $argc and $argv handling
--SKIPIF--
<?php if(ini_get('register_argc_argv') == '') echo 'skip'; ?>
--POST--
--GET--
ab+cd+ef+123+test
--FILE--
<?php 

if (!ini_get('register_globals')) {
	$argc = $_SERVER['argc'];
	$argv = $_SERVER['argv'];
}

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
