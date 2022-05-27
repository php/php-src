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
--EXPECT--
IntlDateFormatter::__construct(): UTC was used as timezone, because the date.timezone value 'Mars/Utopia_Planitia' is invalid
