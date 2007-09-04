<?php
function allowed_rounding_error ($number, $expected) {

	//different machines  may give slightly different floating point numbers
	//This assumes that the results will be the same to + or - 1.0E-10.

	$small_number = 1.0e-10;
	$min = $expected - $small_number;
	$max = $expected + $small_number;

	if ($number < $max  && $number > $min ) {
		return true;
	}
	else {
		return false;
	}
}
?>