--TEST--
mb_ereg_search_setpos() function
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
if (!function_exists("mb_regex_encoding")) die("skip mb_regex_encoding() is not defined");
?>
--FILE--
<?php
mb_regex_encoding('iso-8859-1');
$test_str = 'Iñtërnâtiônàlizætiøn'; // Length = 20

var_dump(mb_ereg_search_setpos(50)); // OK
var_dump(mb_ereg_search_setpos(-1)); // Error

mb_ereg_search_init($test_str);

$positions = array( 5, 20, 21, 25, 0, -5, -20, -30);
foreach($positions as $pos) {
	echo("\n* Position: $pos :\n");
	var_dump(mb_ereg_search_setpos($pos));
	var_dump(mb_ereg_search_getpos());
}
?>
==DONE==
--EXPECTF--
bool(true)

Warning: mb_ereg_search_setpos(): Position is out of range in %s on line %d
bool(false)

* Position: 5 :
bool(true)
int(5)

* Position: 20 :
bool(true)
int(20)

* Position: 21 :

Warning: mb_ereg_search_setpos(): Position is out of range in %s on line %d
bool(false)
int(0)

* Position: 25 :

Warning: mb_ereg_search_setpos(): Position is out of range in %s on line %d
bool(false)
int(0)

* Position: 0 :
bool(true)
int(0)

* Position: -5 :
bool(true)
int(15)

* Position: -20 :
bool(true)
int(0)

* Position: -30 :

Warning: mb_ereg_search_setpos(): Position is out of range in %s on line %d
bool(false)
int(0)
==DONE==
