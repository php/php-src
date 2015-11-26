--TEST--
Bug #40743 (DateTime ignores the TimeZone object passed to the constructor)
--FILE--
<?php
$dt = new DateTime('@1200506699', new DateTimeZone('Europe/Berlin'));
echo $dt->format(DATE_RFC822), "\n";
echo $dt->format('T e Z'), "\n";
echo "-----\n";

date_default_timezone_set('America/New_York');

$dt = new DateTime('16 Jan 08 13:04:59');
echo $dt->format(DATE_RFC822 . " e T O U"), "\n";

$dt = new DateTime('@1200506699');
echo $dt->format(DATE_RFC822 . " e T O U"), "\n";

$dt = new DateTime('@1200506699');
$dt->setTimezone( new DateTimeZone( 'America/New_York' ) );
echo $dt->format(DATE_RFC822 . " e T O U"), "\n";

$dt = new DateTime('@1200506699', new DateTimeZone('Europe/Berlin'));
echo $dt->format(DATE_RFC822 . " e T O U"), "\n";

$dt = new DateTime('16 Jan 08 13:04:59 America/Chicago');
echo $dt->format(DATE_RFC822 . " e T O U"), "\n";

$dt = new DateTime('16 Jan 08 13:04:59 America/Chicago', new DateTimeZone('Europe/Berlin'));
echo $dt->format(DATE_RFC822 . " e T O U"), "\n";
?>
--EXPECT--
Wed, 16 Jan 08 18:04:59 +0000
GMT+0000 +00:00 0
-----
Wed, 16 Jan 08 13:04:59 -0500 America/New_York EST -0500 1200506699
Wed, 16 Jan 08 18:04:59 +0000 +00:00 GMT+0000 +0000 1200506699
Wed, 16 Jan 08 13:04:59 -0500 America/New_York EST -0500 1200506699
Wed, 16 Jan 08 18:04:59 +0000 +00:00 GMT+0000 +0000 1200506699
Wed, 16 Jan 08 13:04:59 -0600 America/Chicago CST -0600 1200510299
Wed, 16 Jan 08 13:04:59 -0600 America/Chicago CST -0600 1200510299
