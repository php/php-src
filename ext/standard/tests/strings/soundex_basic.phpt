--TEST--
Test soundex() function : basic functionality
--FILE--
<?php
/* Prototype  : string soundex  ( string $str  )
 * Description: Calculate the soundex key of a string
 * Source code: ext/standard/string.c
*/
echo "*** Testing soundex() : basic functionality ***\n";

var_dump(soundex("Euler"));
var_dump(soundex("Gauss"));  
var_dump(soundex("Hilbert"));  
var_dump(soundex("Knuth")); 
var_dump(soundex("Lloyd"));
var_dump(soundex("Lukasiewicz"));

var_dump(soundex("Euler")       == soundex("Ellery"));    // E460
var_dump(soundex("Gauss")       == soundex("Ghosh"));     // G200
var_dump(soundex("Hilbert")     == soundex("Heilbronn")); // H416
var_dump(soundex("Knuth")       == soundex("Kant"));      // K530
var_dump(soundex("Lloyd")       == soundex("Ladd"));      // L300
var_dump(soundex("Lukasiewicz") == soundex("Lissajous")); // L222

var_dump(soundex("Lukasiewicz") == soundex("Ghosh"));
var_dump(soundex("Hilbert") == soundex("Ladd"));  
?> 
===DONE===
--EXPECT--
*** Testing soundex() : basic functionality ***
string(4) "E460"
string(4) "G200"
string(4) "H416"
string(4) "K530"
string(4) "L300"
string(4) "L222"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
 
===DONE===
