<?php
$before = "ABC\\ABC";
$expect  = "ABC\\\\ABC";
$after = pg_escape_string($before);
if ($expect === $after) {
	echo "pg_escape_string() is Ok\n";
}
else {
	echo "pg_escape_string() is NOT Ok\n";
	var_dump($before);
	var_dump($after);
	var_dump($expect);
}

$before = "ABC\\ABC";
$expect  = "ABC\\\\\\\\ABC";
$after  = pg_escape_bytea($before);
if ($expect === $after) {
	echo "pg_escape_bytea() is Ok\n";
}
else {
	echo "pg_escape_byte() is NOT Ok\n";
	var_dump($before);
	var_dump($after);
	var_dump($expect);
}

?>
