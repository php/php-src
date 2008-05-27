--TEST--
Test strip_tags() function : basic functionality - with default arguments
--INI--
short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string 
 * Source code: ext/standard/string.c
*/

echo "*** Testing strip_tags() : basic functionality ***\n";

// array of arguments 
$string_array = array (
  "<html>hello</html>",
  '<html>hello</html>',
  "<?php echo hello ?>",
  '<?php echo hello ?>',
  "<? echo hello ?>",
  '<? echo hello ?>',
  "<% echo hello %>",
  '<% echo hello %>',
  "<script language=\"PHP\"> echo hello </script>",
  '<script language=\"PHP\"> echo hello </script>',
  "<html><b>hello</b><p>world</p></html>",
  '<html><b>hello</b><p>world</p></html>',
  "<html><!-- COMMENT --></html>",
  '<html><!-- COMMENT --></html>'
);
  
  		
// Calling strip_tags() with default arguments
// loop through the $string_array to test strip_tags on various inputs
$iteration = 1;
foreach($string_array as $string)
{
  echo "-- Iteration $iteration --\n";
  var_dump( strip_tags($string) );
  $iteration++;
}

echo "Done";
?>
--EXPECT--
*** Testing strip_tags() : basic functionality ***
-- Iteration 1 --
unicode(5) "hello"
-- Iteration 2 --
unicode(5) "hello"
-- Iteration 3 --
unicode(0) ""
-- Iteration 4 --
unicode(0) ""
-- Iteration 5 --
unicode(0) ""
-- Iteration 6 --
unicode(0) ""
-- Iteration 7 --
unicode(0) ""
-- Iteration 8 --
unicode(0) ""
-- Iteration 9 --
unicode(12) " echo hello "
-- Iteration 10 --
unicode(12) " echo hello "
-- Iteration 11 --
unicode(10) "helloworld"
-- Iteration 12 --
unicode(10) "helloworld"
-- Iteration 13 --
unicode(0) ""
-- Iteration 14 --
unicode(0) ""
Done
