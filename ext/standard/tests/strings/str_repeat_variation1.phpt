--TEST--
Test str_repeat() function: usage variations - complex strings containing other than 7-bit chars
--INI--
precision=14
--FILE--
<?php
$str = chr(0).chr(128).chr(129).chr(234).chr(235).chr(254).chr(255);

$withCodePoint = str_repeat($str, chr(51)); // ASCII value of '3' given
$explicit = str_repeat($str, 3);

var_dump($withCodePoint === $explicit);
var_dump( bin2hex( $withCodePoint ) );
var_dump( bin2hex( $explicit ) );

?>
DONE
--EXPECT--
bool(true)
string(42) "008081eaebfeff008081eaebfeff008081eaebfeff"
string(42) "008081eaebfeff008081eaebfeff008081eaebfeff"
DONE
