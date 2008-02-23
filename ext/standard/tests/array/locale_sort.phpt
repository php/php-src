--TEST--
Sort with SORT_LOCALE_STRING
--SKIPIF--
<?php
if (false == setlocale(LC_CTYPE, "fr_FR", "fr_FR.ISO8859-1")) {
  die("skip setlocale() failed\n");
}
?>
--INI--
unicode.script_encoding=ISO8859-1
unicode.output_encoding=ISO8859-1
--FILE--
<?php
setlocale(LC_ALL, 'fr_FR', 'fr_FR.ISO8859-1');
$table = array("AB" => "Alberta",
"BC" => "Colombie-Britannique",
"MB" => "Manitoba",
"NB" => "Nouveau-Brunswick",
"NL" => "Terre-Neuve-et-Labrador",
"NS" => "Nouvelle-Écosse",
"ON" => "Ontario",
"PE" => "Île-du-Prince-Édouard",
"QC" => "Québec",
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
  string(21) "Île-du-Prince-Édouard"
  ["MB"]=>
  string(8) "Manitoba"
  ["NB"]=>
  string(17) "Nouveau-Brunswick"
  ["NS"]=>
  string(15) "Nouvelle-Écosse"
  ["NU"]=>
  string(7) "Nunavut"
  ["ON"]=>
  string(7) "Ontario"
  ["QC"]=>
  string(6) "Québec"
  ["SK"]=>
  string(12) "Saskatchewan"
  ["NL"]=>
  string(23) "Terre-Neuve-et-Labrador"
  ["YT"]=>
  string(19) "Territoire du Yukon"
  ["NT"]=>
  string(25) "Territoires du Nord-Ouest"
}
--UEXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
array(13) {
  [u"AB"]=>
  unicode(7) "Alberta"
  [u"BC"]=>
  unicode(20) "Colombie-Britannique"
  [u"PE"]=>
  unicode(21) "Île-du-Prince-Édouard"
  [u"MB"]=>
  unicode(8) "Manitoba"
  [u"NB"]=>
  unicode(17) "Nouveau-Brunswick"
  [u"NS"]=>
  unicode(15) "Nouvelle-Écosse"
  [u"NU"]=>
  unicode(7) "Nunavut"
  [u"ON"]=>
  unicode(7) "Ontario"
  [u"QC"]=>
  unicode(6) "Québec"
  [u"SK"]=>
  unicode(12) "Saskatchewan"
  [u"NL"]=>
  unicode(23) "Terre-Neuve-et-Labrador"
  [u"YT"]=>
  unicode(19) "Territoire du Yukon"
  [u"NT"]=>
  unicode(25) "Territoires du Nord-Ouest"
}
