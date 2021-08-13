--TEST--
Test strrchr() function : usage variations - binary safe
--FILE--
<?php
/* Test strrchr() function: with binary values & null terminated strings passed to 'str1' & 'str2' */

echo "*** Test strrchr() function: binary safe ***\n";
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
  //needle as null string
  var_dump( strrchr($haystacks[$index], "\0") );
  //needle as empty string
  var_dump( strrchr($haystacks[$index], "") );
}
echo "*** Done ***";
?>
--EXPECTF--
*** Test strrchr() function: binary safe ***
string(6) "%0World"
string(6) "%0World"
string(12) "%0Hello World"
string(12) "%0Hello World"
string(1) "%0"
string(1) "%0"
string(1) "%0"
string(1) "%0"
string(6) "%0world"
string(6) "%0world"
string(6) "%0Hello"
string(6) "%0Hello"
string(1) "%0"
string(1) "%0"
*** Done ***
