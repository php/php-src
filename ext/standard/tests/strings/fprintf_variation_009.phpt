--TEST--
Test fprintf() function (variation - 9)
--FILE--
<?php

$string_variation = array( "%5s", "%-5s", "%05s", "%'#5s" );
$strings = array( NULL, "abc", 'aaa' );

/* creating dumping file */
$data_file = __DIR__ . '/fprintf_variation_009.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

$counter = 1;
/* string type variations */
fprintf($fp, "\n*** Testing fprintf() for string types ***\n");
foreach( $string_variation as $string_var ) {
  fprintf( $fp, "\n-- Iteration %d --\n",$counter);
  foreach( $strings as $str ) {
    fprintf( $fp, "\n");
    fprintf( $fp, $string_var, $str );
  }
  $counter++;
}

fclose($fp);

print_r(file_get_contents($data_file));
echo "\nDone";

unlink($data_file);

?>
--EXPECT--
*** Testing fprintf() for string types ***

-- Iteration 1 --

     
  abc
  aaa
-- Iteration 2 --

     
abc  
aaa  
-- Iteration 3 --

00000
00abc
00aaa
-- Iteration 4 --

#####
##abc
##aaa
Done
