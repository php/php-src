--TEST--
Testing rtrim() function
--FILE--
<?php

/* Testing the Normal behaviour of rtrim() function */

 echo "\n *** Output for Normal Behaviour ***\n";
 var_dump ( rtrim("rtrim test   \t\0 ") );                       /* without second Argument */
 var_dump ( rtrim("rtrim test   " , "") );                       /* no characters in second Argument */
 var_dump ( rtrim("rtrim test        ", true) );                 /* with boolean value as second Argument */
 var_dump ( rtrim("rtrim test        ", " ") );                  /* with single space as second Argument */
 var_dump ( rtrim("rtrim test \t\n\r\0\x0B", "\t\n\r\0\x0B") );  /* with multiple escape sequences as second Argument */
 var_dump ( rtrim("rtrim testABCXYZ", "A..Z") );                 /* with characters range as second Argument */
 var_dump ( rtrim("rtrim test0123456789", "0..9") );             /* with numbers range as second Argument */
 var_dump ( rtrim("rtrim test$#@", "#@$") );                     /* with some special characters as second Argument */


/* Use of class and objects */
echo "\n*** Checking with OBJECTS ***\n";
class string1 {
  public function __toString() {
    return "Object";
  }
}
$obj = new string1;
var_dump( rtrim($obj, "tc") );

/* String with embedded NULL */
echo "\n*** String with embedded NULL ***\n";
var_dump( rtrim("234\x0005678\x0000efgh\xijkl\x0n1", "\x0n1") );

/* heredoc string */
$str = <<<EOD
us
ing heredoc string
EOD;

echo "\n *** Using heredoc string ***\n";
var_dump( rtrim($str, "ing") );

echo "Done\n";
?>
--EXPECTF--
*** Output for Normal Behaviour ***
string(10) "rtrim test"
string(13) "rtrim test   "
string(18) "rtrim test        "
string(10) "rtrim test"
string(11) "rtrim test "
string(10) "rtrim test"
string(10) "rtrim test"
string(10) "rtrim test"

*** Checking with OBJECTS ***
string(4) "Obje"

*** String with embedded NULL ***
string(22) "234%005678%000efgh\xijkl"

 *** Using heredoc string ***
string(18) "us
ing heredoc str"
Done
