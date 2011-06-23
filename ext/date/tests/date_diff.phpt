--TEST--
Extensive test for date_diff().
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$ok = 0;
define( 'COUNT', 120 );
$d0 = new DateTime('2009-11-20');
for ( $i = 0; $i < COUNT * 12; $i++ )
{
	$d = clone $d0;
	$dates[$i] = $d->add( new DateInterval( "P{$i}D" ) );
}

for ( $i = 0; $i < COUNT; $i++)
{
//	echo $dates[$i]->format( "Y-m-d\n" );
	for ( $j = 0; $j < COUNT * 12; $j++)
	{
		$diff = date_diff( $dates[$i], $dates[$j] );
		/*
		printf( "\t%s %s %3d %s\n",
			$dates[$i]->format( 'Y-m-d' ),
			$dates[$j]->format( 'Y-m-d' ),
			$diff->format( '%a' ),
			$diff->format( '%y-%m-%d' )
		);
		*/

		$current = clone $dates[$i];
		$int = new DateInterval( $diff->format( 'P%yY%mM%dD' ) );
		if ( $current > $dates[$j] )
		{
			$current->sub( $int );
		}
		else
		{
			$current->add( $int );
		}
		if ( $current != $dates[$j] )
		{
			echo "FAIL: ",
				$dates[$i]->format( 'Y-m-d' ), " + ",
				$int->format( '%y-%m-%d' ), " = ",
				$current->format( 'Y-m-d' ), " (",
				$dates[$j]->format( 'Y-m-d' ), ")\n";
		}
		else
		{
			$ok++;
		}
	}
}

echo $ok, "\n";
?>
--EXPECT--
172800
