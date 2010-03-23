--TEST--
Test stripos() function : usage variations - null terminated strings for 'needle' argument
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function with null terminated strings for 'needle' argument 
 *  in order to check binary safe 
*/

echo "*** Test stripos() function: binary safe ***\n";
$haystack = "\0Hello\0World\0";

$needles = array(
  "Hello".chr(0)."World",
  chr(0)."Hello World",
  "Hello World".chr(0),
  chr(0).chr(0).chr(0),
  "Hello\0world",
  "\0Hello",
  "Hello\0"
);

for($index = 0; $index < count($needles); $index++ ) {
  var_dump( stripos($haystack, $needles[$index]) );
  var_dump( stripos($haystack, $needles[$index], $index) );
}
echo "*** Done ***";
?>
--EXPECTF--
*** Test stripos() function: binary safe ***
int(1)
int(1)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
bool(false)
int(0)
bool(false)
int(1)
bool(false)
*** Done ***
