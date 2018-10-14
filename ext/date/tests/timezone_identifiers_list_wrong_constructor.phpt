--TEST--
timezone_identifiers_list: Test that correct notice is given when timezone_identifiers_list is given 4096 as parameter
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
error_reporting=E_ALL
date.timezone=UTC
--FILE--
<?php
print_r(timezone_identifiers_list(4096));

?>
--EXPECTF--
Notice: timezone_identifiers_list(): A two-letter ISO 3166-1 compatible country code is expected in %s on line %d
