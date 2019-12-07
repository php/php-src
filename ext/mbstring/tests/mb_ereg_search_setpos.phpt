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

try {
    var_dump(mb_ereg_search_setpos(-1)); // Error
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

mb_ereg_search_init($test_str);

$positions = array( 5, 20, 21, 25, 0, -5, -20, -30);
foreach($positions as $pos) {
	echo("\n* Position: $pos :\n");
	
    try {
	    var_dump(mb_ereg_search_setpos($pos));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

	var_dump(mb_ereg_search_getpos());
}
?>
--EXPECT--
bool(true)
Position is out of range

* Position: 5 :
bool(true)
int(5)

* Position: 20 :
bool(true)
int(20)

* Position: 21 :
Position is out of range
int(20)

* Position: 25 :
Position is out of range
int(20)

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
Position is out of range
int(0)
