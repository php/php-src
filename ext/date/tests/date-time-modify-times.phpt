--TEST--
Test for DateTime::modify() with absolute time statements
--INI--
date.timezone=Europe/London
--FILE--
<?php
$tests = array(
	'2010-12-15 19:42:45 UTC' => array(
		'october 23:00', // October 23rd, with a broken time
		'back of 4pm',
		'next week monday',
		'next week monday 10am',
		'tuesday noon',
		'first monday of January 2011',
		'first monday of January 2011 09:00',
	),
	'2010-12-15 19:42:45' => array(
		'october 23:00', // October 23rd, with a broken time
		'march 28, 00:15',
		'march 28, 01:15', // doesn't exist bcause of DST
		'march 28, 02:15',
	),
);

foreach ( $tests as $start => $data )
{
	foreach ( $data as $test )
	{
		echo date_create( $start )
			->modify( $test )
			->format( DateTime::RFC2822 ), "\n";
	}
}
echo "\n";
?>
--EXPECT--
Sat, 23 Oct 2010 00:00:00 +0000
Wed, 15 Dec 2010 16:15:00 +0000
Mon, 20 Dec 2010 00:00:00 +0000
Mon, 20 Dec 2010 10:00:00 +0000
Tue, 21 Dec 2010 12:00:00 +0000
Mon, 03 Jan 2011 00:00:00 +0000
Mon, 03 Jan 2011 09:00:00 +0000
Sat, 23 Oct 2010 00:00:00 +0100
Sun, 28 Mar 2010 00:15:00 +0000
Sun, 28 Mar 2010 02:15:00 +0100
Sun, 28 Mar 2010 02:15:00 +0100
