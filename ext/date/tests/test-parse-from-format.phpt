--TEST--
date_create_from_format() and date_parse_from_format().
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$formats = array( DATE_ATOM, DATE_COOKIE, DATE_ISO8601, DATE_RFC822,
		DATE_RFC850, DATE_RFC1036, DATE_RFC1123, DATE_RFC2822, DATE_RFC3339,
		DATE_RSS, DATE_W3C );

foreach( $formats as $format )
{
	$date = new DateTime( "2008-07-08T22:14:12+02:00" );
	$formatted = $date->format( $format ) ;
	$date2 = date_create_from_format( $format, $formatted );
	var_dump( $format, $formatted, $date2 );
	echo "\n";
	if ( $date2 === false )
	{
		var_dump(date_parse_from_format( $format, $formatted ) );
	}
}
?>
--EXPECT--
string(13) "Y-m-d\TH:i:sP"
string(25) "2008-07-08T22:14:12+02:00"
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "l, d-M-Y H:i:s T"
string(38) "Tuesday, 08-Jul-2008 22:14:12 GMT+0200"
object(DateTime)#1 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(13) "Y-m-d\TH:i:sO"
string(24) "2008-07-08T22:14:12+0200"
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "D, d M y H:i:s O"
string(29) "Tue, 08 Jul 08 22:14:12 +0200"
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "l, d-M-y H:i:s T"
string(36) "Tuesday, 08-Jul-08 22:14:12 GMT+0200"
object(DateTime)#1 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "D, d M y H:i:s O"
string(29) "Tue, 08 Jul 08 22:14:12 +0200"
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "D, d M Y H:i:s O"
string(31) "Tue, 08 Jul 2008 22:14:12 +0200"
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "D, d M Y H:i:s O"
string(31) "Tue, 08 Jul 2008 22:14:12 +0200"
object(DateTime)#1 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(13) "Y-m-d\TH:i:sP"
string(25) "2008-07-08T22:14:12+02:00"
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(16) "D, d M Y H:i:s O"
string(31) "Tue, 08 Jul 2008 22:14:12 +0200"
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}

string(13) "Y-m-d\TH:i:sP"
string(25) "2008-07-08T22:14:12+02:00"
object(DateTime)#1 (3) {
  ["date"]=>
  string(19) "2008-07-08 22:14:12"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}
