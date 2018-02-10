<?php

if (!function_exists('setlocale')) {
	die('skip: setlocale() not available');
}

if (!@setlocale(LC_ALL, 'pt_PT', 'pt', 'pt_PT.ISO8859-1', 'portuguese')) {
	die('skip pt locale not available');
}
