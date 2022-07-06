--TEST--
Test nl2br() function : usage variations - html values for 'str' argument
--FILE--
<?php
/*
* Test nl2br() function by passing html string inputs containing line breaks and
*   new line chars for 'str'
*/

echo "*** Testing nl2br() : usage variations ***\n";

//array of html strings
$strings = array(
  "<html>Hello<br />world</html>",
  "<html><br /></html>",
  "<html>\nHello\r\nworld\r</html>",
  "<html>\n \r\n \r</html>",
);

//loop through $strings array to test nl2br() function with each element
foreach( $strings as $str ){
  var_dump(nl2br($str) );
}
echo "Done";
?>
--EXPECT--
*** Testing nl2br() : usage variations ***
string(29) "<html>Hello<br />world</html>"
string(19) "<html><br /></html>"
string(45) "<html><br />
Hello<br />
world<br /></html>"
string(37) "<html><br />
 <br />
 <br /></html>"
Done
