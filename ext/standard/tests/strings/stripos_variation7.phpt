--TEST--
Test stripos() function : usage variations - empty heredoc string for 'haystack' argument
--FILE--
<?php
/* Test stripos() function by passing empty heredoc string for haystack
 *  and with various needles & offsets
*/

echo "*** Testing stripos() function: with heredoc strings ***\n";
echo "-- With empty heredoc string --\n";
$empty_string = <<<EOD
EOD;
var_dump( stripos($empty_string, "") );

try {
    stripos($empty_string, "", 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump( stripos($empty_string, FALSE) );
var_dump( stripos($empty_string, NULL) );

echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: with heredoc strings ***
-- With empty heredoc string --
int(0)
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
int(0)
int(0)
*** Done ***
