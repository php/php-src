--TEST--
Test is_callable() function : usage variations - defined functions
--FILE--
<?php
function check_iscallable( $functions ) {
  $counter = 1;
  foreach($functions as $func) {
    echo "-- Iteration  $counter --\n";
    var_dump( is_callable($func) );  //given only $var argument
    var_dump( is_callable($func, TRUE) );  //given $var and $syntax argument
    var_dump( is_callable($func, TRUE, $callable_name) );
    echo $callable_name, "\n";
    var_dump( is_callable($func, FALSE) );  //given $var and $syntax argument
    var_dump( is_callable($func, FALSE, $callable_name) );
    echo $callable_name, "\n";
    $counter++;
  }
}

echo "\n*** Testing is_callable() on defined functions ***\n";
/* function name with simple string */
function someFunction() {
}

/* function name with mixed string and integer */
function x123() {
}

/* function name as NULL */
function NULL() {
}

/* function name with boolean name */
function false() {
}

/* function name with string and special character */
function Hello_World() {
}

$defined_functions = array (
  $functionVar1 = 'someFunction',
  $functionVar2 = 'x123',
  $functionVar3 = 'NULL',
  $functionVar4 = 'false',
  $functionVar5 = "Hello_World"
);
/* use check_iscallable() to check whether given string is valid function name
 *  expected: true as it is valid callback
 */
check_iscallable($defined_functions);

?>
--EXPECT--
*** Testing is_callable() on defined functions ***
-- Iteration  1 --
bool(true)
bool(true)
bool(true)
someFunction
bool(true)
bool(true)
someFunction
-- Iteration  2 --
bool(true)
bool(true)
bool(true)
x123
bool(true)
bool(true)
x123
-- Iteration  3 --
bool(true)
bool(true)
bool(true)
NULL
bool(true)
bool(true)
NULL
-- Iteration  4 --
bool(true)
bool(true)
bool(true)
false
bool(true)
bool(true)
false
-- Iteration  5 --
bool(true)
bool(true)
bool(true)
Hello_World
bool(true)
bool(true)
Hello_World
