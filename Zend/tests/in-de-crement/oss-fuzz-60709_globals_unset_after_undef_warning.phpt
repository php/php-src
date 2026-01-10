--TEST--
oss-fuzz #60709: Unsetting variable after undefined variable warning in ++/--
--FILE--
<?php
set_error_handler(function($_, $m) {
    echo "$m\n";
    unset($GLOBALS['x']);
});

echo "POST DEC\n";
var_dump($x--);
unset($x);
echo "POST INC\n";
var_dump($x++);
unset($x);
echo "PRE DEC\n";
var_dump(--$x);
unset($x);
echo "PRE INC\n";
var_dump(++$x);
?>
--EXPECT--
POST DEC
Undefined variable $x
Decrement on type null has no effect, this will change in the next major version of PHP
NULL
POST INC
Undefined variable $x
NULL
PRE DEC
Undefined variable $x
Decrement on type null has no effect, this will change in the next major version of PHP
NULL
PRE INC
Undefined variable $x
int(1)
