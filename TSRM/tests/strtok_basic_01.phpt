--TEST--
Test the basics to function strtok.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php
$string = "This is\tan example\nstring";

$tok = strtok($string," \n\t");

while ($tok !== false) {
    var_dump("Word=$tok<br/>");
    $tok = strtok(" \n\t");
}
?>
--EXPECT--
string(14) "Word=This<br/>"
string(12) "Word=is<br/>"
string(12) "Word=an<br/>"
string(17) "Word=example<br/>"
string(16) "Word=string<br/>"
