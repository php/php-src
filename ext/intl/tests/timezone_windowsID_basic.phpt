--TEST--
IntlTimeZone::getWindowsID basic test
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '52') < 0)die('skip for ICU >= 52'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '58.1') >= 0) die('skip for ICU <= 57.1'); ?>
--FILE--
<?php

$tzs = array(
  'America/Bogota',
  'America/Havana',
  'America/Indiana/Knox',
  'America/Los_Angeles',
  'Azeroth/Kalimdor/Durotar',
  'Africa/Casablanca',
  'Asia/Singapore',
  'Australia/Perth',
  'Europe/London',
  'Europe/Istanbul',
);

foreach ($tzs as $tz) {
  var_dump(IntlTimeZone::getWindowsID($tz));
  if (intl_get_error_code() != U_ZERO_ERROR) {
    echo "Error: ", intl_get_error_message(), "\n";
  }
}
?>
--EXPECT--
string(24) "SA Pacific Standard Time"
string(21) "Eastern Standard Time"
string(21) "Central Standard Time"
string(21) "Pacific Standard Time"
bool(false)
Error: intltz_get_windows_id: Unknown system timezone: U_ILLEGAL_ARGUMENT_ERROR
string(21) "Morocco Standard Time"
string(23) "Singapore Standard Time"
string(26) "W. Australia Standard Time"
string(17) "GMT Standard Time"
string(20) "Turkey Standard Time"
