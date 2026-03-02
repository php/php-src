--TEST--
Test strrpos() function : usage variations - empty heredoc string for 'haystack' argument
--FILE--
<?php
/* Test strrpos() function by passing empty heredoc string for haystack
 *  and with various needles & offsets
*/

echo "*** Testing strrpos() function: with heredoc strings ***\n";
echo "-- With empty heredoc string --\n";
$empty_string = <<<EOD
EOD;
var_dump( strrpos($empty_string, "") );
try {
    strrpos($empty_string, "", 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump( strrpos($empty_string, FALSE) );

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function: with heredoc strings ***
-- With empty heredoc string --
int(0)
strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
int(0)
*** Done ***
