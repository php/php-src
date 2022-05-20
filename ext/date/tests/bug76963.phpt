--TEST--
Bug #76963 (Null-byte injection in CreateFromFormat and related functions)
--FILE--
<?php
$strings = [
	'8/8/2016',
	"8/8/2016\0asf",
];

foreach ($strings as $string) {
	echo "Covering string: ", htmlspecialchars( $string), "\n";

	try {
		$d1 = DateTime::createFromFormat('m/d/Y', $string);
	} catch (ValueError $v) {
		echo $v->getMessage(), "\n";
	}

	try {
		$d2 = DateTimeImmutable::createFromFormat('m/d/Y', $string);
	} catch (ValueError $v) {
		echo $v->getMessage(), "\n";
	}

	try {
		$d3 = date_parse_from_format('m/d/Y', $string);
	} catch (ValueError $v) {
		echo $v->getMessage(), "\n";
	}

	var_dump($d1, $d2, $d3);
}
