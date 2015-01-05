--TEST--
strtotime() function (32 bit)
--SKIPIF--
<?php echo PHP_INT_SIZE == 8 ? "skip 32-bit only" : "OK"; ?>
--FILE--
<?php
date_default_timezone_set('Europe/Lisbon');
$time = 1150494719; // 16/June/2006

$strs = array(
	'',
	" \t\r\n000",
	'yesterday',
	'22:49:12',
	'22:49:12 bogusTZ',
	'22.49.12.42GMT',
	'22.49.12.42bogusTZ',
	't0222',
	't0222 t0222',
	'022233',
	'022233 bogusTZ',
	'2-3-2004',
	'2.3.2004',
	'20060212T23:12:23UTC',
	'20060212T23:12:23 bogusTZ',
	'2006167', //pgydotd
	'Jan-15-2006', //pgtextshort
	'2006-Jan-15', //pgtextreverse
	'10/Oct/2000:13:55:36 +0100', //clf
	'10/Oct/2000:13:55:36 +00100', //clf
	'2006',
	'1986', // year
	'JAN',
	'January',
);

foreach ($strs as $str) {
	$t = strtotime($str, $time);
	if (is_integer($t)) {
		var_dump(date(DATE_RFC2822, $t));
	} else {
		var_dump($t);
	}
}

?>
--EXPECT--
bool(false)
bool(false)
string(31) "Thu, 15 Jun 2006 00:00:00 +0100"
string(31) "Fri, 16 Jun 2006 22:49:12 +0100"
bool(false)
string(31) "Fri, 16 Jun 2006 23:49:12 +0100"
bool(false)
string(31) "Fri, 16 Jun 2006 02:22:00 +0100"
bool(false)
string(31) "Fri, 16 Jun 2006 02:22:33 +0100"
bool(false)
string(31) "Tue, 02 Mar 2004 00:00:00 +0000"
string(31) "Tue, 02 Mar 2004 00:00:00 +0000"
string(31) "Sun, 12 Feb 2006 23:12:23 +0000"
bool(false)
string(31) "Fri, 16 Jun 2006 00:00:00 +0100"
string(31) "Sun, 15 Jan 2006 00:00:00 +0000"
string(31) "Sun, 15 Jan 2006 00:00:00 +0000"
string(31) "Tue, 10 Oct 2000 13:55:36 +0100"
bool(false)
string(31) "Fri, 16 Jun 2006 20:06:00 +0100"
string(31) "Mon, 16 Jun 1986 22:51:59 +0100"
string(31) "Mon, 16 Jan 2006 00:00:00 +0000"
string(31) "Mon, 16 Jan 2006 00:00:00 +0000"
