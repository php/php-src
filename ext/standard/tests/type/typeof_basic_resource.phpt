--TEST--
typeof() correctly recognizes resource vars
--FILE--
<?php

$closed = tmpfile();
fclose($closed);

$data = [
	STDIN,
	fopen(__FILE__, 'r'),
	opendir(__DIR__),
	$closed,
];

echo "\n*** typeof test: resource ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum));
}

echo "\n*** typeof test extended: resource ***\n";
foreach ($data as $datum) {
	var_dump(typeof($datum, true));
}

?>
--EXPECTF--

*** typeof test: resource ***
string(8) "resource"
string(8) "resource"
string(8) "resource"
string(8) "resource"

*** typeof test extended: resource ***
string(23) "resource of type stream"
string(23) "resource of type stream"
string(23) "resource of type stream"
string(15) "closed resource"
