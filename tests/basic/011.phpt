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
--EXPECT--
0: ab
1: cd
2: ef
3: 123
4: test
