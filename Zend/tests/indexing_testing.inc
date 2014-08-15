<?php
function run_test($variable, $value, $withKey) {
	if ($variable !== 'undefined') {
		$testing = $variable;
	}
	if ($withKey) {
		$testing['foo'] = $value;
	}else {
		$testing[] = $value;
	}
	var_dump($testing);
}

function run_test_ref($variable, &$value, $withKey) {
	if ($variable !== 'undefined') {
		$testing = $variable;
	}
	if ($withKey) {
		$testing['foo'] = &$value;
	}else {
		$testing[] = &$value;
	}
	var_dump($testing);
}
