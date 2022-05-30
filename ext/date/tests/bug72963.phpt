--TEST--
Bug #72963 (Null-byte injection in CreateFromFormat and related functions)
--INI--
date.timezone=UTC
--FILE--
<?php
$strings = [
	'8/8/2016',
	"8/8/2016\0asf",
];

foreach ($strings as $string) {
	$d1 = $d2 = $d3 = NULL;
	echo "\nCovering string: ", addslashes($string), "\n\n";

	try {
		$d1 = DateTime::createFromFormat('!m/d/Y', $string);
	} catch (ValueError $v) {
		echo $v->getMessage(), "\n";
	}

	try {
		$d2 = DateTimeImmutable::createFromFormat('!m/d/Y', $string);
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
?>
--EXPECT--
Covering string: 8/8/2016

object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2016-08-08 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTimeImmutable)#2 (3) {
  ["date"]=>
  string(26) "2016-08-08 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
array(12) {
  ["year"]=>
  int(2016)
  ["month"]=>
  int(8)
  ["day"]=>
  int(8)
  ["hour"]=>
  bool(false)
  ["minute"]=>
  bool(false)
  ["second"]=>
  bool(false)
  ["fraction"]=>
  bool(false)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}

Covering string: 8/8/2016\0asf

DateTime::createFromFormat(): Argument #2 ($datetime) must not contain any null bytes
DateTimeImmutable::createFromFormat(): Argument #2 ($datetime) must not contain any null bytes
date_parse_from_format(): Argument #2 ($datetime) must not contain any null bytes
NULL
NULL
NULL
