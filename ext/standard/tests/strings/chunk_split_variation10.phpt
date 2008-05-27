--TEST--
Test chunk_split() function : usage variations - different single quoted strings for 'ending' argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* passing different single quoted strings for 'ending' arguments to chunk_split()
*/

echo "*** Testing chunk_split() : different single quoted strings as 'ending' ***\n";


//Initializing variables
$str = "This is to test chunk_split() with various 'single quoted' ending string.";
$chunklen = 9.2;

//different values for 'ending' argument
$values = array (
  '',  //empty
  ' ',  //space
  'a',  //Single char
  'ENDING',  //String
  '@#$%^',  //Special chars

  
  '\t',  
  '\n',
  '\r',
  '\r\n',
  
  '\0',  //Null char
  '123',  //Numeric
  '(MSG)',  //With ( and )
  ') ending string (',  //sentence as ending string
  ') numbers 1234 (',  //string with numbers
  ') speci@! ch@r$ ('  //string with special chars
);


//loop through each element of values for 'ending'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration $count --\n";
  var_dump( chunk_split($str, $chunklen, $values[$count]) );
}

echo "Done"
?>
--EXPECT--
*** Testing chunk_split() : different single quoted strings as 'ending' ***
-- Iteration 0 --
unicode(73) "This is to test chunk_split() with various 'single quoted' ending string."
-- Iteration 1 --
unicode(82) "This is t o test ch unk_split () with v arious 's ingle quo ted' endi ng string . "
-- Iteration 2 --
unicode(82) "This is tao test chaunk_splita() with vaarious 'saingle quoated' endiang stringa.a"
-- Iteration 3 --
unicode(127) "This is tENDINGo test chENDINGunk_splitENDING() with vENDINGarious 'sENDINGingle quoENDINGted' endiENDINGng stringENDING.ENDING"
-- Iteration 4 --
unicode(118) "This is t@#$%^o test ch@#$%^unk_split@#$%^() with v@#$%^arious 's@#$%^ingle quo@#$%^ted' endi@#$%^ng string@#$%^.@#$%^"
-- Iteration 5 --
unicode(91) "This is t\to test ch\tunk_split\t() with v\tarious 's\tingle quo\tted' endi\tng string\t.\t"
-- Iteration 6 --
unicode(91) "This is t\no test ch\nunk_split\n() with v\narious 's\ningle quo\nted' endi\nng string\n.\n"
-- Iteration 7 --
unicode(91) "This is t\ro test ch\runk_split\r() with v\rarious 's\ringle quo\rted' endi\rng string\r.\r"
-- Iteration 8 --
unicode(109) "This is t\r\no test ch\r\nunk_split\r\n() with v\r\narious 's\r\ningle quo\r\nted' endi\r\nng string\r\n.\r\n"
-- Iteration 9 --
unicode(91) "This is t\0o test ch\0unk_split\0() with v\0arious 's\0ingle quo\0ted' endi\0ng string\0.\0"
-- Iteration 10 --
unicode(100) "This is t123o test ch123unk_split123() with v123arious 's123ingle quo123ted' endi123ng string123.123"
-- Iteration 11 --
unicode(118) "This is t(MSG)o test ch(MSG)unk_split(MSG)() with v(MSG)arious 's(MSG)ingle quo(MSG)ted' endi(MSG)ng string(MSG).(MSG)"
-- Iteration 12 --
unicode(226) "This is t) ending string (o test ch) ending string (unk_split) ending string (() with v) ending string (arious 's) ending string (ingle quo) ending string (ted' endi) ending string (ng string) ending string (.) ending string ("
-- Iteration 13 --
unicode(217) "This is t) numbers 1234 (o test ch) numbers 1234 (unk_split) numbers 1234 (() with v) numbers 1234 (arious 's) numbers 1234 (ingle quo) numbers 1234 (ted' endi) numbers 1234 (ng string) numbers 1234 (.) numbers 1234 ("
-- Iteration 14 --
unicode(226) "This is t) speci@! ch@r$ (o test ch) speci@! ch@r$ (unk_split) speci@! ch@r$ (() with v) speci@! ch@r$ (arious 's) speci@! ch@r$ (ingle quo) speci@! ch@r$ (ted' endi) speci@! ch@r$ (ng string) speci@! ch@r$ (.) speci@! ch@r$ ("
Done
