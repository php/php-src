--TEST--
IntlDateFormat constructor failure
--INI--
date.timezone=Mars/Utopia_Planitia
--EXTENSIONS--
intl
--FILE--
<?php

try {
  new \IntlDateFormatter('en_US', \IntlDateFormatter::FULL, \IntlDateFormatter::FULL);
  echo "Wat?";
} catch (\IntlException $e) {
  echo $e->getMessage();
}
?>
--EXPECTF--
Warning: PHP Startup: Invalid date.timezone value 'Mars/Utopia_Planitia', using 'UTC' instead in %s on line %d
Wat?
