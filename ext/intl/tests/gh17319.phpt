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

$bundle = (new ResourceBundle('', NULL))->get('calendar')->get('buddhist');
$iterator = $bundle->getIterator();
while ($iterator->valid()) {
	var_dump($iterator->key());
	$iterator->next();
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
KEY: string(11) "appendItems"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(16) "availableFormats"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(8) "dayNames"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(4) "eras"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(15) "intervalFormats"
VALUE: object(ResourceBundle)#4 (0) {
}
KEY: string(10) "monthNames"
VALUE: object(ResourceBundle)#3 (0) {
}
KEY: string(8) "quarters"
VALUE: object(ResourceBundle)#4 (0) {
}
string(15) "AmPmMarkersAbbr"
string(15) "AmPmMarkersAbbr"
string(16) "DateTimePatterns"
string(11) "appendItems"
string(16) "availableFormats"
string(8) "dayNames"
string(4) "eras"
string(15) "intervalFormats"
string(10) "monthNames"
string(8) "quarters"
