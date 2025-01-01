--TEST--
GH-17319 (ResourceBundle iterator crash on NULL key)
--EXTENSIONS--
intl
--CREDITS--
KidFlo
--FILE--
<?php
foreach ((new ResourceBundle('', NULL))->get('calendar')->get('buddhist') as $key => $value) {
	echo "KEY: "; var_dump($key);
	echo "VALUE: "; var_dump($value);
}
?>
--EXPECT--
KEY: string(15) "AmPmMarkersAbbr"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(15) "AmPmMarkersAbbr"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(16) "DateTimePatterns"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY:
Warning: Undefined variable $key in /home/dcarlier/Contribs/php-src/ext/intl/tests/gh17317.php on line 3
NULL
VALUE:
Warning: Undefined variable $value in /home/dcarlier/Contribs/php-src/ext/intl/tests/gh17317.php on line 4
NULL
KEY:
Warning: Undefined variable $key in /home/dcarlier/Contribs/php-src/ext/intl/tests/gh17317.php on line 3
NULL
VALUE:
Warning: Undefined variable $value in /home/dcarlier/Contribs/php-src/ext/intl/tests/gh17317.php on line 4
NULL
KEY: string(11) "appendItems"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(16) "availableFormats"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(8) "dayNames"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(4) "eras"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(15) "intervalFormats"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(10) "monthNames"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(8) "quarters"
VALUE: object(ResourceBundle)#3 (0) {
}
