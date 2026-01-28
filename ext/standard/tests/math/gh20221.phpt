--TEST--
GH-20221: max/min return wrong value if signed (negative) zero
--FILE--
<?php

foreach ([-0.0, 0.0] as $a) {
	foreach ([-0.0, 0.0] as $b) {
		if ("$a $b" === "0 0" || "$a $b" === "-0 -0") {
			continue;
		}

		printf("min(%.17g, %.17g)   = %.17g\n", $a, $b, min($a, $b));
		printf("min([%.17g, %.17g]) = %.17g\n", $a, $b, min([$a, $b]));
		printf("max(%.17g, %.17g)   = %.17g\n", $a, $b, max($a, $b));
		printf("max([%.17g, %.17g]) = %.17g\n", $a, $b, max([$a, $b]));
		echo "\n";
	}
}

echo "======\n\n";

foreach ([-0.0, 0.0] as $a) {
	foreach ([-0.0, 0.0] as $b) {
		foreach ([-0.0, 0.0] as $c) {
			if ("$a $b $c" === "0 0 0" || "$a $b $c" === "-0 -0 -0") {
				continue;
			}

			printf("min(%.17g, %.17g, %.17g)   = %.17g\n", $a, $b, $c, min($a, $b, $c));
			printf("min([%.17g, %.17g, %.17g]) = %.17g\n", $a, $b, $c, min([$a, $b, $c]));
			printf("max(%.17g, %.17g, %.17g)   = %.17g\n", $a, $b, $c, max($a, $b, $c));
			printf("max([%.17g, %.17g, %.17g]) = %.17g\n", $a, $b, $c, max([$a, $b, $c]));
			echo "\n";
		}
	}
}

var_dump(min( 0, -0.0 ));
var_dump(min( -0.0, 0 ));
var_dump(max( 0, -0.0 ));
var_dump(max( -0.0, 0 ));

?>
--EXPECT--
min(-0, 0)   = -0
min([-0, 0]) = -0
max(-0, 0)   = 0
max([-0, 0]) = 0

min(0, -0)   = -0
min([0, -0]) = -0
max(0, -0)   = 0
max([0, -0]) = 0

======

min(-0, -0, 0)   = -0
min([-0, -0, 0]) = -0
max(-0, -0, 0)   = 0
max([-0, -0, 0]) = 0

min(-0, 0, -0)   = -0
min([-0, 0, -0]) = -0
max(-0, 0, -0)   = 0
max([-0, 0, -0]) = 0

min(-0, 0, 0)   = -0
min([-0, 0, 0]) = -0
max(-0, 0, 0)   = 0
max([-0, 0, 0]) = 0

min(0, -0, -0)   = -0
min([0, -0, -0]) = -0
max(0, -0, -0)   = 0
max([0, -0, -0]) = 0

min(0, -0, 0)   = -0
min([0, -0, 0]) = -0
max(0, -0, 0)   = 0
max([0, -0, 0]) = 0

min(0, 0, -0)   = -0
min([0, 0, -0]) = -0
max(0, 0, -0)   = 0
max([0, 0, -0]) = 0

float(-0)
float(-0)
int(0)
int(0)
