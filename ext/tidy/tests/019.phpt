--TEST--
tidy_repair_*() and invalid parameters
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php

$l = 1;
$s = "";
$a = array();

tidy_repair_string($s, $l, $l, $l);
tidy_repair_string($s, $s, $s, $s);
tidy_repair_string($l, $l, $l ,$l);
tidy_repair_string($a, $a, $a, $a);

tidy_repair_file($s, $l, $l, $l);
tidy_repair_file($s, $s, $s, $s);
tidy_repair_file($l, $l, $l ,$l);
tidy_repair_file($a, $a, $a, $a);

echo "Done\n";
?>
--EXPECTF--
Warning: tidy_repair_string(): Could not load configuration file '1' in %s on line %d

Warning: tidy_repair_string(): Could not set encoding '1' in %s on line %d

Warning: tidy_repair_string(): Could not load configuration file '' in %s on line %d

Warning: tidy_repair_string(): Could not load configuration file '1' in %s on line %d

Warning: tidy_repair_string(): Could not set encoding '1' in %s on line %d

Warning: tidy_repair_string() expects parameter 1 to be string, array given in %s on line %d

Warning: tidy_repair_file(): Filename cannot be empty in %s on line %d

Warning: tidy_repair_file(): Filename cannot be empty in %s on line %d

Warning: tidy_repair_file() expects parameter 1 to be a valid path, array given in %s on line %d
Done
