--TEST--
Test strrpos() function : usage variations - checking binary safe with 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function with null terminated strings for 'haystack' argument
 *  in order to check the binary safe
*/

echo "*** Test strrpos() function: binary safe ***\n";
$haystacks = array(
  "Hello".chr(0)."World",
  chr(0)."Hello World",
  "Hello World".chr(0),
  chr(0).chr(0).chr(0),
  "Hello\0world",
  "\0Hello",
  "Hello\0"
);

for($index = 0; $index < count($haystacks); $index++ ) {
  var_dump( strrpos($haystacks[$index], "\0") );
  var_dump( strrpos($haystacks[$index], "\0", $index) );
}
echo "*** Done ***";
?>
--EXPECT--
*** Test strrpos() function: binary safe ***
int(5)
int(5)
int(0)
bool(false)
int(11)
int(11)
int(2)
bool(false)
int(5)
int(5)
int(0)
bool(false)
int(5)
bool(false)
*** Done ***
