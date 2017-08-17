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

var_dump($n = count( $collect ));
echo ( $n > 700 ) ? "microseconds differ\n" : "microseconds do not differ enough ($n)\n";
?>
--EXPECTF--
int(%d)
microseconds differ
