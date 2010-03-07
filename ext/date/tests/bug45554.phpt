--TEST--
Bug #45554 (Inconsistent behavior of the u format char)
--INI--
date.timezone=UTC
--FILE--
<?php
$format = "m-d-Y H:i:s.u T";
$d = date_create_from_format($format, "03-15-2005 12:22:29.000000 PST");
echo $d->format($format), "\n";

$d = date_create_from_format($format, "03-15-2005 12:22:29.001001 PST");
echo $d->format($format), " (precision isn't enough to show the 1 here)\n";

$d = date_create_from_format($format, "03-15-2005 12:22:29.0010 PST");
echo $d->format($format), "\n";
?>
--EXPECT--
03-15-2005 12:22:29.000000 PST
03-15-2005 12:22:29.001000 PST (precision isn't enough to show the 1 here)
03-15-2005 12:22:29.001000 PST
