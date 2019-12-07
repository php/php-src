--TEST--
Bug #45866 (decimal values fed to DateTime->modify() causes long execution times)
--INI--
date.timezone=UTC
--FILE--
<?php
$date = new DateTime( '2009-07-29 16:44:23 Europe/London' );
$date->modify( "+1.61538461538 day" );
echo $date->format( 'r' ), "\n";

$date = new DateTime( '2009-07-29 16:44:23 Europe/London' );
$date->modify( "61538461538 day" );
echo $date->format( 'r' ), "\n";

$date = new DateTime( '2009-07-29 16:44:23 Europe/London' );

try {
    $date->modify( "£61538461538 day" );
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

echo $date->format( 'r' ), "\n";
?>
--EXPECTF--
Thu, 14 Aug 168488594 16:44:23 +0000
Thu, 14 Aug 168488594 16:44:23 +0000
Error: Failed to parse time string (£61538461538 day) at position 0 (%s): Unexpected character
Wed, 29 Jul 2009 16:44:23 +0100
