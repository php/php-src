--TEST--
Test strip_tags() function : basic functionality - with default arguments
--FILE--
<?php
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
string(5) "hello"
-- Iteration 2 --
string(5) "hello"
-- Iteration 3 --
string(0) ""
-- Iteration 4 --
string(0) ""
-- Iteration 5 --
string(0) ""
-- Iteration 6 --
string(0) ""
-- Iteration 7 --
string(0) ""
-- Iteration 8 --
string(0) ""
-- Iteration 9 --
string(12) " echo hello "
-- Iteration 10 --
string(12) " echo hello "
-- Iteration 11 --
string(10) "helloworld"
-- Iteration 12 --
string(10) "helloworld"
-- Iteration 13 --
string(0) ""
-- Iteration 14 --
string(0) ""
Done
