--TEST--
IntlTimeZone::getIDForWindowsID basic test
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '58.1') < 0) die('skip for ICU >= 58.1'); ?>
--FILE--
<?php

$tzs = array(
  'Gnomeregan' => array(NULL),
  'India Standard Time' => array(NULL),
  'Pacific Standard Time' => array('001', 'CA', 'MX', 'US', 'ZZ'),
  'Romance Standard Time' => array('001', 'BE', 'DK', 'ES', 'FR'),
);

foreach ($tzs as $tz => $regions) {
  echo "** $tz\n";
  foreach ($regions as $region) {
    var_dump(IntlTimeZone::getIDForWindowsID($tz, $region));
    if (intl_get_error_code() != U_ZERO_ERROR) {
      echo "Error: ", intl_get_error_message(), "\n";
    }
  }
}
?>
--EXPECT--
** Gnomeregan
bool(false)
Error: unknown windows timezone: U_ILLEGAL_ARGUMENT_ERROR
** India Standard Time
string(13) "Asia/Calcutta"
** Pacific Standard Time
string(19) "America/Los_Angeles"
string(17) "America/Vancouver"
string(19) "America/Los_Angeles"
string(19) "America/Los_Angeles"
string(7) "PST8PDT"
** Romance Standard Time
string(12) "Europe/Paris"
string(15) "Europe/Brussels"
string(17) "Europe/Copenhagen"
string(13) "Europe/Madrid"
string(12) "Europe/Paris"
