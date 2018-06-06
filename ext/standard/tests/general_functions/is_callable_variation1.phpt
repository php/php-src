--TEST--
Test is_callable() function : usage variations - undefined functions
--SKIPIF--
<?php
if (!extension_loaded('json')) die("skip requires ext/json");
?>
--FILE--
<?php
/* Prototype: bool is_callable ( mixed $var [, bool $syntax_only [, string &$callable_name]] );
   Description: Verify that the contents of a variable can be called as a function
                In case of objects, $var = array($SomeObject, 'MethodName')
*/

/* Prototype: void check_iscallable( $functions );
   Description: use iscallable() on given string to check for valid function name
                returns true if valid function name, false otherwise
*/
function check_iscallable( $functions ) {
  $counter = 1;
  foreach($functions as $func) {
    echo "-- Iteration  $counter --\n";
    var_dump( is_callable($func) );  //given only $var argument
    var_dump( is_callable($func, TRUE) );  //given $var and $syntax argument
    var_dump( is_callable($func, TRUE, $callable_name) );
    echo json_encode($callable_name) . "\n";
    var_dump( is_callable($func, FALSE) );  //given $var and $syntax argument
    var_dump( is_callable($func, FALSE, $callable_name) );
    echo json_encode($callable_name) . "\n";
    $counter++;
  }
}

echo "\n*** Testing is_callable() on undefined functions ***\n";
$undef_functions = array (
  "",  //empty string
  '',
  " ",  //string with a space
  ' ',
  "12356",
  "\0",
  '\0',
  "hello world",
  'hello world',
  "welcome\0",
  'welcome\0',
  "==%%%***$$$@@@!!",
  "false",
  "\070",
  '\t',  //escape character
  '\007',
  '123',
  'echo()'
);

/* use check_iscallable() to check whether given string is valid function name
 * expected: true with $syntax = TRUE
 *           false with $syntax = FALSE
 */
check_iscallable($undef_functions);

?>
===DONE===
--EXPECT--
*** Testing is_callable() on undefined functions ***
-- Iteration  1 --
bool(false)
bool(true)
bool(true)
""
bool(false)
bool(false)
""
-- Iteration  2 --
bool(false)
bool(true)
bool(true)
""
bool(false)
bool(false)
""
-- Iteration  3 --
bool(false)
bool(true)
bool(true)
" "
bool(false)
bool(false)
" "
-- Iteration  4 --
bool(false)
bool(true)
bool(true)
" "
bool(false)
bool(false)
" "
-- Iteration  5 --
bool(false)
bool(true)
bool(true)
"12356"
bool(false)
bool(false)
"12356"
-- Iteration  6 --
bool(false)
bool(true)
bool(true)
"\u0000"
bool(false)
bool(false)
"\u0000"
-- Iteration  7 --
bool(false)
bool(true)
bool(true)
"\\0"
bool(false)
bool(false)
"\\0"
-- Iteration  8 --
bool(false)
bool(true)
bool(true)
"hello world"
bool(false)
bool(false)
"hello world"
-- Iteration  9 --
bool(false)
bool(true)
bool(true)
"hello world"
bool(false)
bool(false)
"hello world"
-- Iteration  10 --
bool(false)
bool(true)
bool(true)
"welcome\u0000"
bool(false)
bool(false)
"welcome\u0000"
-- Iteration  11 --
bool(false)
bool(true)
bool(true)
"welcome\\0"
bool(false)
bool(false)
"welcome\\0"
-- Iteration  12 --
bool(false)
bool(true)
bool(true)
"==%%%***$$$@@@!!"
bool(false)
bool(false)
"==%%%***$$$@@@!!"
-- Iteration  13 --
bool(false)
bool(true)
bool(true)
"false"
bool(false)
bool(false)
"false"
-- Iteration  14 --
bool(false)
bool(true)
bool(true)
"8"
bool(false)
bool(false)
"8"
-- Iteration  15 --
bool(false)
bool(true)
bool(true)
"\\t"
bool(false)
bool(false)
"\\t"
-- Iteration  16 --
bool(false)
bool(true)
bool(true)
"\\007"
bool(false)
bool(false)
"\\007"
-- Iteration  17 --
bool(false)
bool(true)
bool(true)
"123"
bool(false)
bool(false)
"123"
-- Iteration  18 --
bool(false)
bool(true)
bool(true)
"echo()"
bool(false)
bool(false)
"echo()"
===DONE===
