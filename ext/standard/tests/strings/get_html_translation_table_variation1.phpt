--TEST--
Test get_html_translation_table() function : usage variations - unexpected table values
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == "WIN"){  
  die('skip Not for Windows');
}

if( !setlocale(LC_ALL, "en_US.UTF-8") ) {
  die('skip failed to set locale settings to "en-US.UTF-8"');
}
?>
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/*
 * test get_html_translation_table() with unexpected value for argument $table 
*/

//set locale to en_US.UTF-8
setlocale(LC_ALL, "en_US.UTF-8");

echo "*** Testing get_html_translation_table() : usage variations ***\n";
// initialize all required variables
$quote_style = ENT_COMPAT;

// get an unset variable
$unset_var = 10;
unset($unset_var);

// a resource variable 
$fp = fopen(__FILE__, "r");

// array with different values
$values =  array (

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // string values
  "string",
  'string',

  // objects
  new stdclass(),

  // empty string
  "",
  '',

  // null vlaues
  NULL,
  null,

  // resource var
  $fp,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of get_html_translation_table()
// when $table arugment is supplied with different values
echo "\n--- Testing get_html_translation_table() by supplying different values for 'table' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $table = $values [$index];

  var_dump( get_html_translation_table($table) );
  var_dump( get_html_translation_table($table, $quote_style) );

  $counter ++;
}

// close resource
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : usage variations ***

--- Testing get_html_translation_table() by supplying different values for 'table' argument ---
-- Iteration 1 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL
-- Iteration 2 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL
-- Iteration 3 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL
-- Iteration 4 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL
-- Iteration 5 --

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, array given in %s on line %s
NULL
-- Iteration 6 --
array(100) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["×"]=>
  string(7) "&times;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["ß"]=>
  string(7) "&szlig;"
  ["à"]=>
  string(8) "&agrave;"
  ["á"]=>
  string(8) "&aacute;"
  ["â"]=>
  string(7) "&acirc;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["å"]=>
  string(7) "&aring;"
  ["æ"]=>
  string(7) "&aelig;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["è"]=>
  string(8) "&egrave;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["ë"]=>
  string(6) "&euml;"
  ["ì"]=>
  string(8) "&igrave;"
  ["í"]=>
  string(8) "&iacute;"
  ["î"]=>
  string(7) "&icirc;"
  ["ï"]=>
  string(6) "&iuml;"
  ["ð"]=>
  string(5) "&eth;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ò"]=>
  string(8) "&ograve;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["õ"]=>
  string(8) "&otilde;"
  ["ö"]=>
  string(6) "&ouml;"
  ["÷"]=>
  string(8) "&divide;"
  ["ø"]=>
  string(8) "&oslash;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["ú"]=>
  string(8) "&uacute;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ü"]=>
  string(6) "&uuml;"
  ["ý"]=>
  string(8) "&yacute;"
  ["þ"]=>
  string(7) "&thorn;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(100) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["×"]=>
  string(7) "&times;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["ß"]=>
  string(7) "&szlig;"
  ["à"]=>
  string(8) "&agrave;"
  ["á"]=>
  string(8) "&aacute;"
  ["â"]=>
  string(7) "&acirc;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["å"]=>
  string(7) "&aring;"
  ["æ"]=>
  string(7) "&aelig;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["è"]=>
  string(8) "&egrave;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["ë"]=>
  string(6) "&euml;"
  ["ì"]=>
  string(8) "&igrave;"
  ["í"]=>
  string(8) "&iacute;"
  ["î"]=>
  string(7) "&icirc;"
  ["ï"]=>
  string(6) "&iuml;"
  ["ð"]=>
  string(5) "&eth;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ò"]=>
  string(8) "&ograve;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["õ"]=>
  string(8) "&otilde;"
  ["ö"]=>
  string(6) "&ouml;"
  ["÷"]=>
  string(8) "&divide;"
  ["ø"]=>
  string(8) "&oslash;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["ú"]=>
  string(8) "&uacute;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ü"]=>
  string(6) "&uuml;"
  ["ý"]=>
  string(8) "&yacute;"
  ["þ"]=>
  string(7) "&thorn;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 7 --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 8 --
array(100) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["×"]=>
  string(7) "&times;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["ß"]=>
  string(7) "&szlig;"
  ["à"]=>
  string(8) "&agrave;"
  ["á"]=>
  string(8) "&aacute;"
  ["â"]=>
  string(7) "&acirc;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["å"]=>
  string(7) "&aring;"
  ["æ"]=>
  string(7) "&aelig;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["è"]=>
  string(8) "&egrave;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["ë"]=>
  string(6) "&euml;"
  ["ì"]=>
  string(8) "&igrave;"
  ["í"]=>
  string(8) "&iacute;"
  ["î"]=>
  string(7) "&icirc;"
  ["ï"]=>
  string(6) "&iuml;"
  ["ð"]=>
  string(5) "&eth;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ò"]=>
  string(8) "&ograve;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["õ"]=>
  string(8) "&otilde;"
  ["ö"]=>
  string(6) "&ouml;"
  ["÷"]=>
  string(8) "&divide;"
  ["ø"]=>
  string(8) "&oslash;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["ú"]=>
  string(8) "&uacute;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ü"]=>
  string(6) "&uuml;"
  ["ý"]=>
  string(8) "&yacute;"
  ["þ"]=>
  string(7) "&thorn;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(100) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["×"]=>
  string(7) "&times;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["ß"]=>
  string(7) "&szlig;"
  ["à"]=>
  string(8) "&agrave;"
  ["á"]=>
  string(8) "&aacute;"
  ["â"]=>
  string(7) "&acirc;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["å"]=>
  string(7) "&aring;"
  ["æ"]=>
  string(7) "&aelig;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["è"]=>
  string(8) "&egrave;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["ë"]=>
  string(6) "&euml;"
  ["ì"]=>
  string(8) "&igrave;"
  ["í"]=>
  string(8) "&iacute;"
  ["î"]=>
  string(7) "&icirc;"
  ["ï"]=>
  string(6) "&iuml;"
  ["ð"]=>
  string(5) "&eth;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ò"]=>
  string(8) "&ograve;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["õ"]=>
  string(8) "&otilde;"
  ["ö"]=>
  string(6) "&ouml;"
  ["÷"]=>
  string(8) "&divide;"
  ["ø"]=>
  string(8) "&oslash;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["ú"]=>
  string(8) "&uacute;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ü"]=>
  string(6) "&uuml;"
  ["ý"]=>
  string(8) "&yacute;"
  ["þ"]=>
  string(7) "&thorn;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 9 --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 10 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL
-- Iteration 11 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL
-- Iteration 12 --

Warning: get_html_translation_table() expects parameter 1 to be long, object given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, object given in %s on line %s
NULL
-- Iteration 13 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL
-- Iteration 14 --

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, string given in %s on line %s
NULL
-- Iteration 15 --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 16 --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 17 --

Warning: get_html_translation_table() expects parameter 1 to be long, resource given in %s on line %s
NULL

Warning: get_html_translation_table() expects parameter 1 to be long, resource given in %s on line %s
NULL
-- Iteration 18 --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- Iteration 19 --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
Done
