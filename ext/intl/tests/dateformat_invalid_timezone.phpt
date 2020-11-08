--TEST--
IntlDateFormat constructor failure
--INI--
date.timezone=Mars/Utopia_Planitia
--SKIPIF--
<?php
extension_loaded('intl') || print 'skip';
--FILE--
<?php

try {
  new \IntlDateFormatter('en_US', \IntlDateFormatter::FULL, \IntlDateFormatter::FULL);
  echo "Wat?";
} catch (\IntlException $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
IntlDateFormatter::__construct(): Invalid date.timezone value 'Mars/Utopia_Planitia', we selected the timezone 'UTC' for now.
