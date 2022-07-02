--TEST--
Tests for DateTimeImmutable.
--INI--
date.timezone=Europe/London
--FILE--
<?php
$tz = new DateTimeZone("Asia/Tokyo");
$current = "2012-12-27 16:24:08";

function dump($a, $b, $c)
{
    echo 'orig:    ', $a->format('Y-m-d H:i:s e'), "\n";
    echo 'copy:    ', $b->format('Y-m-d H:i:s e'), "\n";
    echo 'changed: ', $c->format('Y-m-d H:i:s e'), "\n";
}

echo "modify():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->modify("+2 days");
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->modify("+2 days");
dump($v, $z, $x);

echo "\nadd():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->add(new DateInterval("P2DT2S"));
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->add(new DateInterval("P2DT2S"));
dump($v, $z, $x);

echo "\nsub():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->sub(new DateInterval("P2DT2S"));
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->sub(new DateInterval("P2DT2S"));
dump($v, $z, $x);

echo "\nsetTimezone():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->setTimezone($tz);
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->setTimezone($tz);
dump($v, $z, $x);
$v = new DateTimeImmutable($current);
$z = $v;
$x = $z->setTimezone($tz);
dump($v, $z, $x);

echo "\nsetTime():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->setTime(5, 7, 19);
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->setTime(5, 7, 19);
dump($v, $z, $x);

echo "\nsetDate():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->setDate(5, 7, 19);
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->setDate(5, 7, 19);
dump($v, $z, $x);

echo "\nsetIsoDate():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->setIsoDate(2012, 2, 6);
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->setIsoDate(2012, 2, 6);
dump($v, $z, $x);

echo "\nsetTimestamp():\n";
$v = date_create_immutable($current);
$z = $v;
$x = $z->setTimestamp(2012234222);
dump($v, $z, $x);
$v = date_create($current);
$z = $v;
$x = $z->setTimestamp(2012234222);
dump($v, $z, $x);
?>
--EXPECT--
modify():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-12-29 16:24:08 Europe/London
orig:    2012-12-29 16:24:08 Europe/London
copy:    2012-12-29 16:24:08 Europe/London
changed: 2012-12-29 16:24:08 Europe/London

add():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-12-29 16:24:10 Europe/London
orig:    2012-12-29 16:24:10 Europe/London
copy:    2012-12-29 16:24:10 Europe/London
changed: 2012-12-29 16:24:10 Europe/London

sub():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-12-25 16:24:06 Europe/London
orig:    2012-12-25 16:24:06 Europe/London
copy:    2012-12-25 16:24:06 Europe/London
changed: 2012-12-25 16:24:06 Europe/London

setTimezone():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-12-28 01:24:08 Asia/Tokyo
orig:    2012-12-28 01:24:08 Asia/Tokyo
copy:    2012-12-28 01:24:08 Asia/Tokyo
changed: 2012-12-28 01:24:08 Asia/Tokyo
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-12-28 01:24:08 Asia/Tokyo

setTime():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-12-27 05:07:19 Europe/London
orig:    2012-12-27 05:07:19 Europe/London
copy:    2012-12-27 05:07:19 Europe/London
changed: 2012-12-27 05:07:19 Europe/London

setDate():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 0005-07-19 16:24:08 Europe/London
orig:    0005-07-19 16:24:08 Europe/London
copy:    0005-07-19 16:24:08 Europe/London
changed: 0005-07-19 16:24:08 Europe/London

setIsoDate():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2012-01-14 16:24:08 Europe/London
orig:    2012-01-14 16:24:08 Europe/London
copy:    2012-01-14 16:24:08 Europe/London
changed: 2012-01-14 16:24:08 Europe/London

setTimestamp():
orig:    2012-12-27 16:24:08 Europe/London
copy:    2012-12-27 16:24:08 Europe/London
changed: 2033-10-06 18:57:02 Europe/London
orig:    2033-10-06 18:57:02 Europe/London
copy:    2033-10-06 18:57:02 Europe/London
changed: 2033-10-06 18:57:02 Europe/London
