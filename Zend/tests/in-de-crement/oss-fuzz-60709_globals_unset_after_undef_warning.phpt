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
NULL
Undefined variable $x
Decrement on type null has no effect, this will change in the next major version of PHP
POST INC
NULL
Undefined variable $x
PRE DEC
NULL
Undefined variable $x
Decrement on type null has no effect, this will change in the next major version of PHP
PRE INC
int(1)
Undefined variable $x
