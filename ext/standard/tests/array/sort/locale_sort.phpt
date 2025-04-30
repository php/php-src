--TEST--
Sort with SORT_LOCALE_STRING
--SKIPIF--
<?php
if (setlocale(LC_ALL, 'invalid') === 'invalid') { die('skip setlocale() is broken /w musl'); }
if (substr(PHP_OS, 0, 3) == 'WIN') {
  die("skip Unix locale name only, not available on windows (and crashes with VC6)\n");
}
if (false == setlocale(LC_CTYPE, "fr_FR.ISO8859-1", "fr_FR")) {
  die("skip setlocale() failed\n");
}
?>
--FILE--
<?php
setlocale(LC_ALL, 'fr_FR.ISO8859-1', 'fr_FR');
$table = array("AB" => "Alberta",
"BC" => "Colombie-Britannique",
"MB" => "Manitoba",
"NB" => "Nouveau-Brunswick",
"NL" => "Terre-Neuve-et-Labrador",
"NS" => "Nouvelle-�cosse",
"ON" => "Ontario",
"PE" => "�le-du-Prince-�douard",
"QC" => "Qu�bec",
"SK" => "Saskatchewan",
"NT" => "Territoires du Nord-Ouest",
"NU" => "Nunavut",
"YT" => "Territoire du Yukon");
asort($table, SORT_LOCALE_STRING);
var_dump($table);
?>
--EXPECT--
array(13) {
  ["AB"]=>
  string(7) "Alberta"
  ["BC"]=>
  string(20) "Colombie-Britannique"
  ["PE"]=>
  string(21) "�le-du-Prince-�douard"
  ["MB"]=>
  string(8) "Manitoba"
  ["NB"]=>
  string(17) "Nouveau-Brunswick"
  ["NS"]=>
  string(15) "Nouvelle-�cosse"
  ["NU"]=>
  string(7) "Nunavut"
  ["ON"]=>
  string(7) "Ontario"
  ["QC"]=>
  string(6) "Qu�bec"
  ["SK"]=>
  string(12) "Saskatchewan"
  ["NL"]=>
  string(23) "Terre-Neuve-et-Labrador"
  ["YT"]=>
  string(19) "Territoire du Yukon"
  ["NT"]=>
  string(25) "Territoires du Nord-Ouest"
}
