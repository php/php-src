--TEST--
file_get_contents() test using negative parameter for length (last parameter)
--CREDITS--
"Blanche V.N." <valerie_nare@yahoo.fr>
"Sylvain R." <sracine@phpquebec.org>
--INI--
display_errors=false
--FILE--
<?php
try {
    file_get_contents("http://checkip.dyndns.com",null,null,0,-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
file_get_contents(): Argument #5 ($length) must be greater than or equal to 0
