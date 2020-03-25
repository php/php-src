--TEST--
tidy_repair_*() and invalid parameters
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php

$l = 1;
$s = "";

tidy_repair_string($s, $l, $l);
tidy_repair_string($s, $s, $s);
tidy_repair_string($l, $l, $l);

tidy_repair_file($s, $l, $l, $l);
tidy_repair_file($s, $s, $s, $s);
tidy_repair_file($l, $l, $l ,$l);

echo "Done\n";
?>
--EXPECTF--
Warning: tidy_repair_string(): Could not load configuration file '1' in %s on line %d

Warning: tidy_repair_string(): Could not set encoding '1' in %s on line %d

Warning: tidy_repair_string(): Could not load configuration file '' in %s on line %d

Warning: tidy_repair_string(): Could not load configuration file '1' in %s on line %d

Warning: tidy_repair_string(): Could not set encoding '1' in %s on line %d

Warning: tidy_repair_file(): Filename cannot be empty in %s on line %d

Warning: tidy_repair_file(): Filename cannot be empty in %s on line %d
Done
