--TEST--
Test join() function : usage variations - different values for 'glue' argument
--FILE--
<?php
/*
 * test join() by passing different glue arguments
*/

echo "*** Testing join() : usage variations ***\n";

$glues = array (
  "TRUE",
  true,
  false,
  array("key1", "key2"),
  "",
  " ",
  "string\x00between",
  -1.566599999999999,
  -0,
  '\0'
);

$pieces = array (
  2,
  0,
  -639,
  -1.3444,
  true,
  "PHP",
  false,
  "",
  " ",
  6999.99999999,
  "string\x00with\x00...\0"
);
/* loop through  each element of $glues and call join */
$counter = 1;
for($index = 0; $index < count($glues); $index ++) {
  echo "-- Iteration $counter --\n";
  try {
    var_dump(join($glues[$index], $pieces));
  } catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
  }
  $counter++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : usage variations ***
-- Iteration 1 --
string(87) "2TRUE0TRUE-639TRUE-1.3444TRUE1TRUEPHPTRUETRUETRUE TRUE6999.99999999TRUEstring%0with%0...%0"
-- Iteration 2 --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(57) "2101-6391-1.3444111PHP111 16999.999999991string%0with%0...%0"
-- Iteration 3 --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(47) "20-639-1.34441PHP 6999.99999999string%0with%0...%0"
-- Iteration 4 --
join(): Argument #1 ($separator) must be of type string, array given
-- Iteration 5 --
string(47) "20-639-1.34441PHP 6999.99999999string%0with%0...%0"
-- Iteration 6 --
string(57) "2 0 -639 -1.3444 1 PHP     6999.99999999 string%0with%0...%0"
-- Iteration 7 --
string(187) "2string%0between0string%0between-639string%0between-1.3444string%0between1string%0betweenPHPstring%0betweenstring%0betweenstring%0between string%0between6999.99999999string%0betweenstring%0with%0...%0"
-- Iteration 8 --
string(117) "2-1.56660-1.5666-639-1.5666-1.3444-1.56661-1.5666PHP-1.5666-1.5666-1.5666 -1.56666999.99999999-1.5666string%0with%0...%0"
-- Iteration 9 --
string(57) "2000-6390-1.3444010PHP000 06999.999999990string%0with%0...%0"
-- Iteration 10 --
string(67) "2\00\0-639\0-1.3444\01\0PHP\0\0\0 \06999.99999999\0string%0with%0...%0"
Done
