--TEST--
Bug #51866 (Lenient parsing with parseFromFormat)
--FILE--
<?php
date_default_timezone_set('UTC');

$tests = array(
	array( 'Y-m-d',   '2001-11-29 13:20:01' ),
	array( 'Y-m-d+',  '2001-11-29 13:20:01' ),
	array( 'Y-m-d +', '2001-11-29 13:20:01' ),
	array( 'Y-m-d+',  '2001-11-29' ),
	array( 'Y-m-d +', '2001-11-29' ),
	array( 'Y-m-d +', '2001-11-29 ' ),
);
foreach( $tests as $test )
{
	list($format, $str) = $test;
	var_dump($format, $str);
	$d = DateTime::createFromFormat($format, $str);
	var_dump($d);
	var_dump(DateTime::getLastErrors());

	echo "\n\n";
}
--EXPECTF--
string(5) "Y-m-d"
string(19) "2001-11-29 13:20:01"
bool(false)
array(4) {
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(1)
  ["errors"]=>
  array(1) {
    [10]=>
    string(13) "Trailing data"
  }
}


string(6) "Y-m-d+"
string(19) "2001-11-29 13:20:01"
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2001-11-29 %d:%d:%d"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(%d) "%s"
}
array(4) {
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [10]=>
    string(13) "Trailing data"
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
}


string(7) "Y-m-d +"
string(19) "2001-11-29 13:20:01"
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2001-11-29 %d:%d:%d"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(%d) "%s"
}
array(4) {
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [11]=>
    string(13) "Trailing data"
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
}


string(6) "Y-m-d+"
string(10) "2001-11-29"
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2001-11-29 %d:%d:%d"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(%d) "%s"
}
array(4) {
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
}


string(7) "Y-m-d +"
string(10) "2001-11-29"
bool(false)
array(4) {
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(1)
  ["errors"]=>
  array(1) {
    [10]=>
    string(12) "Data missing"
  }
}


string(7) "Y-m-d +"
string(11) "2001-11-29 "
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2001-11-29 %d:%d:%d"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(%d) "%s"
}
array(4) {
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
}
