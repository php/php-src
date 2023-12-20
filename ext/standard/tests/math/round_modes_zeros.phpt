--TEST--
round() with different rounding modes for zeros
--FILE--
<?php
$modes = [
    "PHP_ROUND_HALF_UP" => PHP_ROUND_HALF_UP,
    "PHP_ROUND_HALF_DOWN" => PHP_ROUND_HALF_DOWN,
    "PHP_ROUND_HALF_EVEN" => PHP_ROUND_HALF_EVEN,
    "PHP_ROUND_HALF_ODD" => PHP_ROUND_HALF_ODD,
    "PHP_ROUND_CEILING" => PHP_ROUND_CEILING,
    "PHP_ROUND_FLOOR" => PHP_ROUND_FLOOR,
    "PHP_ROUND_TOWARD_ZERO" => PHP_ROUND_TOWARD_ZERO,
    "PHP_ROUND_AWAY_FROM_ZERO" => PHP_ROUND_AWAY_FROM_ZERO,
];

$precisions = [-1, 0, 1, 2];

foreach ($modes as $modeKey => $mode) {
    echo "mode: $modeKey\n";
    foreach ($precisions as $precision) {
        echo "\tprecision: $precision\n";
        echo "\t\t     0 => " . round(0, $precision, $mode) . "\n";
        echo "\t\t    -0 => " . round(-0, $precision, $mode) . "\n";
        echo "\t\t   0.0 => " . round(0.0, $precision, $mode) . "\n";
        echo "\t\t  -0.0 => " . round(-0.0, $precision, $mode) . "\n";
        echo "\n";
    }
}

?>
--EXPECT--
mode: PHP_ROUND_HALF_UP
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_HALF_DOWN
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_HALF_EVEN
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_HALF_ODD
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_CEILING
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_FLOOR
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_TOWARD_ZERO
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

mode: PHP_ROUND_AWAY_FROM_ZERO
	precision: -1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 0
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 1
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0

	precision: 2
		     0 => 0
		    -0 => 0
		   0.0 => 0
		  -0.0 => -0
