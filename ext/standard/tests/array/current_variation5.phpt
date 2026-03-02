--TEST--
Test current() function : usage variations - reference & normal parameters
--FILE--
<?php
echo "*** Testing current() : usage variations ***\n";

echo "\n-- Function: reference parameter --\n";

function current_variation5_ref(&$a)
{
    var_dump(current($a));
    var_dump(next($a));
}

$a = array('yes', 'maybe', 'no');

var_dump(current($a));
var_dump(next($a));
current_variation5($a);

echo "\n-- Function: normal parameter --\n";

function current_variation5($a)
{
    var_dump(current($a));
    var_dump(next($a));
}

$a = array('yes', 'maybe', 'no');

var_dump(current($a));
var_dump(next($a));
current_variation5($a);

?>
--EXPECT--
*** Testing current() : usage variations ***

-- Function: reference parameter --
string(3) "yes"
string(5) "maybe"
string(5) "maybe"
string(2) "no"

-- Function: normal parameter --
string(3) "yes"
string(5) "maybe"
string(5) "maybe"
string(2) "no"
