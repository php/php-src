--TEST--
sprintf() function
--POST--
--GET--
--FILE--
<?php 

$agent = sprintf("%.5s", "James Bond, 007");

echo("sprintf string truncate test:   ");
if ($agent == "James") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo("sprintf padding and align test: ");
$test = sprintf("abc%04d %-20s%c", 20, "fisketur", 33);
if ($test == "abc0020 fisketur            !") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo("sprintf octal and hex test:     ");
$test = sprintf("%4o %4x %4X %0"."8x", 128, 1024, 49151, 3457925);
if ($test == " 200  400 BFFF 0034c385") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo("sprintf octal binary test:      ");
$test = sprintf("%b", 3457925);
if ($test == "1101001100001110000101") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo("sprintf float test:             ");
$test = sprintf("%0"."06.2f", 10000/3.0);
if ($test == "003333.33") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo sprintf("%.2f\n", "99.00");
echo sprintf("%.2f\n", 99.00);
?>
--EXPECT--
sprintf string truncate test:   passed
sprintf padding and align test: passed
sprintf octal and hex test:     passed
sprintf octal binary test:      passed
sprintf float test:             passed
99.00
99.00
