--TEST--
imap_timeout() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_timeout();

echo  "Checking with incorrect parameter type\n";
imap_timeout('');
imap_timeout(false);

echo "GET values:\n";
var_dump(imap_timeout(IMAP_OPENTIMEOUT));
var_dump(imap_timeout(IMAP_READTIMEOUT));
var_dump(imap_timeout(IMAP_WRITETIMEOUT));
var_dump(imap_timeout(IMAP_CLOSETIMEOUT));

echo "SET values:\n";
var_dump(imap_timeout(IMAP_OPENTIMEOUT, 10));
var_dump(imap_timeout(IMAP_READTIMEOUT, 10));
var_dump(imap_timeout(IMAP_WRITETIMEOUT, 10));

//IMAP_CLOSETIMEOUT not implemented
//var_dump(imap_timeout(IMAP_CLOSETIMEOUT, 10));

echo "CHECK values:\n";
var_dump(imap_timeout(IMAP_OPENTIMEOUT));
var_dump(imap_timeout(IMAP_READTIMEOUT));
var_dump(imap_timeout(IMAP_WRITETIMEOUT));

//IMAP_CLOSETIMEOUT not implemented
//var_dump(imap_timeout(IMAP_CLOSETIMEOUT));

?>
--EXPECTF--
Checking with no parameters

Warning: imap_timeout() expects at least 1 parameter, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_timeout() expects parameter 1 to be int, %s given in %s on line %d
GET values:
int(%d)
int(%d)
int(%d)
int(%d)
SET values:
bool(true)
bool(true)
bool(true)
CHECK values:
int(10)
int(10)
int(10)
