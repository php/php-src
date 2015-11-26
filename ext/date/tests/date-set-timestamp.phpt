--TEST--
DateTime::setTimestamp()
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$d = new DateTime( '@1217184864' );
echo $d->format( "Y-m-d H:i e\n" );

$d = new DateTime();
$d->setTimestamp( 1217184864 );
echo $d->format( "Y-m-d H:i e\n" );
?>
--EXPECT--
2008-07-27 18:54 +00:00
2008-07-27 20:54 Europe/Oslo
