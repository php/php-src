--TEST--
tidy_repair_*() and invalid parameters
--EXTENSIONS--
tidy
--FILE--
<?php

$l = 1;
$s = "";

tidy_repair_string($s, $l, $l);
tidy_repair_string($s, $s, $s);
tidy_repair_string($l, $l, $l);

try {
    tidy_repair_file($s, $l, $l, $l);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    tidy_repair_file($s, $s, $s, $s);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

tidy_repair_file($l, $l, $l ,$l); // This doesn't emit any warning, TODO look into

echo "Done\n";
?>
--EXPECTF--
Warning: tidy_repair_string(): Could not load the Tidy configuration file "1" in %s on line %d

Warning: tidy_repair_string(): Could not set encoding "1" in %s on line %d

Warning: tidy_repair_string(): Could not load the Tidy configuration file "" in %s on line %d

Warning: tidy_repair_string(): Could not load the Tidy configuration file "1" in %s on line %d

Warning: tidy_repair_string(): Could not set encoding "1" in %s on line %d
Path cannot be empty
Path cannot be empty
Done
