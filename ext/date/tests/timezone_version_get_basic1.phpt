--TEST--
timezone_version_get: Test that timezone_location_get returns a date concatenated with a dot and a version number
--CREDITS--
Rodrigo Wanderley de Melo Cardoso <rodrigogepem@gmail.com>
#PHPTestFest2014 São Paulo 2014-07-05
--INI--
date.timezone=UTC
--FILE--
<?php $versionTimezone = timezone_version_get();
echo $versionTimezone; ?>
--EXPECTREGEX--
^[12][0-9]{3}.[0-9]+$
