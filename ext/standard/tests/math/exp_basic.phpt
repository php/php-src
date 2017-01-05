--TEST--
Test exp() - basic function test for exp()
--INI--
precision=14
--FILE--
<?php
$values = array(10,
				10.3,
				3.9505e3,
				037,
				0x5F,	
				"10",
				"3950.5",
				"3.9505e3",
				"039",
				true,
				false,
				null, 
				);	

$iterator = 1;
foreach($values as $value) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(exp($value));
	$iterator++;
};

?>
===Done===
--EXPECTF--

-- Iteration 1 --
float(22026.465794807)

-- Iteration 2 --
float(29732.618852891)

-- Iteration 3 --
float(INF)

-- Iteration 4 --
float(29048849665247)

-- Iteration 5 --
float(1.811239082889E+41)

-- Iteration 6 --
float(22026.465794807)

-- Iteration 7 --
float(INF)

-- Iteration 8 --
float(INF)

-- Iteration 9 --
float(8.6593400423994E+16)

-- Iteration 10 --
float(2.718281828459)

-- Iteration 11 --
float(1)

-- Iteration 12 --
float(1)
===Done===
