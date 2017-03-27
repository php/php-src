--TEST--
Bug #73837: Milliseconds in DateTime()
--FILE--
<?php
$collect = [];

for ( $i = 0; $i < 1000; $i++ )
{
	$a = new DateTime();
	$key = "s" . $a->format( "u" );
	$collect[$key] = true;
}

echo ( count( $collect ) > 990 ) ? "microseconds differ\n" : "microseconds do not differ enough\n";
?>
--EXPECT--
microseconds differ
