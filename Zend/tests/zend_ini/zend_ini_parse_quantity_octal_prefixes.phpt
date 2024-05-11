--TEST--
Test parsing of octal quantities
--EXTENSIONS--
zend_test
--FILE--
<?php

// This test checks valid formats do not throw any warnings.
foreach (['', ' '] as $leadingWS) {
  foreach (['', '+', '-'] as $sign) {
    foreach (['', ' '] as $midWS) {
      // Ignore G due to overflow on 32bits
      foreach (['', 'K', 'k', 'M', 'm'] as $exp) {
        foreach (['', ' '] as $trailingWS) {
          $setting = sprintf('%s%s0o14%s%s%s',
                             $leadingWS, $sign, $midWS, $exp, $trailingWS);
          printf("# \"%s\"\n", $setting);
          var_dump(zend_test_zend_ini_parse_quantity($setting));
          print "\n";

          $setting = sprintf('%s%s0O14%s%s%s',
                             $leadingWS, $sign, $midWS, $exp, $trailingWS);
          printf("# \"%s\"\n", $setting);
          var_dump(zend_test_zend_ini_parse_quantity($setting));
          print "\n";
        }
      }
    }
  }
}
?>
--EXPECT--
# "0o14"
int(12)

# "0O14"
int(12)

# "0o14 "
int(12)

# "0O14 "
int(12)

# "0o14K"
int(12288)

# "0O14K"
int(12288)

# "0o14K "
int(12288)

# "0O14K "
int(12288)

# "0o14k"
int(12288)

# "0O14k"
int(12288)

# "0o14k "
int(12288)

# "0O14k "
int(12288)

# "0o14M"
int(12582912)

# "0O14M"
int(12582912)

# "0o14M "
int(12582912)

# "0O14M "
int(12582912)

# "0o14m"
int(12582912)

# "0O14m"
int(12582912)

# "0o14m "
int(12582912)

# "0O14m "
int(12582912)

# "0o14 "
int(12)

# "0O14 "
int(12)

# "0o14  "
int(12)

# "0O14  "
int(12)

# "0o14 K"
int(12288)

# "0O14 K"
int(12288)

# "0o14 K "
int(12288)

# "0O14 K "
int(12288)

# "0o14 k"
int(12288)

# "0O14 k"
int(12288)

# "0o14 k "
int(12288)

# "0O14 k "
int(12288)

# "0o14 M"
int(12582912)

# "0O14 M"
int(12582912)

# "0o14 M "
int(12582912)

# "0O14 M "
int(12582912)

# "0o14 m"
int(12582912)

# "0O14 m"
int(12582912)

# "0o14 m "
int(12582912)

# "0O14 m "
int(12582912)

# "+0o14"
int(12)

# "+0O14"
int(12)

# "+0o14 "
int(12)

# "+0O14 "
int(12)

# "+0o14K"
int(12288)

# "+0O14K"
int(12288)

# "+0o14K "
int(12288)

# "+0O14K "
int(12288)

# "+0o14k"
int(12288)

# "+0O14k"
int(12288)

# "+0o14k "
int(12288)

# "+0O14k "
int(12288)

# "+0o14M"
int(12582912)

# "+0O14M"
int(12582912)

# "+0o14M "
int(12582912)

# "+0O14M "
int(12582912)

# "+0o14m"
int(12582912)

# "+0O14m"
int(12582912)

# "+0o14m "
int(12582912)

# "+0O14m "
int(12582912)

# "+0o14 "
int(12)

# "+0O14 "
int(12)

# "+0o14  "
int(12)

# "+0O14  "
int(12)

# "+0o14 K"
int(12288)

# "+0O14 K"
int(12288)

# "+0o14 K "
int(12288)

# "+0O14 K "
int(12288)

# "+0o14 k"
int(12288)

# "+0O14 k"
int(12288)

# "+0o14 k "
int(12288)

# "+0O14 k "
int(12288)

# "+0o14 M"
int(12582912)

# "+0O14 M"
int(12582912)

# "+0o14 M "
int(12582912)

# "+0O14 M "
int(12582912)

# "+0o14 m"
int(12582912)

# "+0O14 m"
int(12582912)

# "+0o14 m "
int(12582912)

# "+0O14 m "
int(12582912)

# "-0o14"
int(-12)

# "-0O14"
int(-12)

# "-0o14 "
int(-12)

# "-0O14 "
int(-12)

# "-0o14K"
int(-12288)

# "-0O14K"
int(-12288)

# "-0o14K "
int(-12288)

# "-0O14K "
int(-12288)

# "-0o14k"
int(-12288)

# "-0O14k"
int(-12288)

# "-0o14k "
int(-12288)

# "-0O14k "
int(-12288)

# "-0o14M"
int(-12582912)

# "-0O14M"
int(-12582912)

# "-0o14M "
int(-12582912)

# "-0O14M "
int(-12582912)

# "-0o14m"
int(-12582912)

# "-0O14m"
int(-12582912)

# "-0o14m "
int(-12582912)

# "-0O14m "
int(-12582912)

# "-0o14 "
int(-12)

# "-0O14 "
int(-12)

# "-0o14  "
int(-12)

# "-0O14  "
int(-12)

# "-0o14 K"
int(-12288)

# "-0O14 K"
int(-12288)

# "-0o14 K "
int(-12288)

# "-0O14 K "
int(-12288)

# "-0o14 k"
int(-12288)

# "-0O14 k"
int(-12288)

# "-0o14 k "
int(-12288)

# "-0O14 k "
int(-12288)

# "-0o14 M"
int(-12582912)

# "-0O14 M"
int(-12582912)

# "-0o14 M "
int(-12582912)

# "-0O14 M "
int(-12582912)

# "-0o14 m"
int(-12582912)

# "-0O14 m"
int(-12582912)

# "-0o14 m "
int(-12582912)

# "-0O14 m "
int(-12582912)

# " 0o14"
int(12)

# " 0O14"
int(12)

# " 0o14 "
int(12)

# " 0O14 "
int(12)

# " 0o14K"
int(12288)

# " 0O14K"
int(12288)

# " 0o14K "
int(12288)

# " 0O14K "
int(12288)

# " 0o14k"
int(12288)

# " 0O14k"
int(12288)

# " 0o14k "
int(12288)

# " 0O14k "
int(12288)

# " 0o14M"
int(12582912)

# " 0O14M"
int(12582912)

# " 0o14M "
int(12582912)

# " 0O14M "
int(12582912)

# " 0o14m"
int(12582912)

# " 0O14m"
int(12582912)

# " 0o14m "
int(12582912)

# " 0O14m "
int(12582912)

# " 0o14 "
int(12)

# " 0O14 "
int(12)

# " 0o14  "
int(12)

# " 0O14  "
int(12)

# " 0o14 K"
int(12288)

# " 0O14 K"
int(12288)

# " 0o14 K "
int(12288)

# " 0O14 K "
int(12288)

# " 0o14 k"
int(12288)

# " 0O14 k"
int(12288)

# " 0o14 k "
int(12288)

# " 0O14 k "
int(12288)

# " 0o14 M"
int(12582912)

# " 0O14 M"
int(12582912)

# " 0o14 M "
int(12582912)

# " 0O14 M "
int(12582912)

# " 0o14 m"
int(12582912)

# " 0O14 m"
int(12582912)

# " 0o14 m "
int(12582912)

# " 0O14 m "
int(12582912)

# " +0o14"
int(12)

# " +0O14"
int(12)

# " +0o14 "
int(12)

# " +0O14 "
int(12)

# " +0o14K"
int(12288)

# " +0O14K"
int(12288)

# " +0o14K "
int(12288)

# " +0O14K "
int(12288)

# " +0o14k"
int(12288)

# " +0O14k"
int(12288)

# " +0o14k "
int(12288)

# " +0O14k "
int(12288)

# " +0o14M"
int(12582912)

# " +0O14M"
int(12582912)

# " +0o14M "
int(12582912)

# " +0O14M "
int(12582912)

# " +0o14m"
int(12582912)

# " +0O14m"
int(12582912)

# " +0o14m "
int(12582912)

# " +0O14m "
int(12582912)

# " +0o14 "
int(12)

# " +0O14 "
int(12)

# " +0o14  "
int(12)

# " +0O14  "
int(12)

# " +0o14 K"
int(12288)

# " +0O14 K"
int(12288)

# " +0o14 K "
int(12288)

# " +0O14 K "
int(12288)

# " +0o14 k"
int(12288)

# " +0O14 k"
int(12288)

# " +0o14 k "
int(12288)

# " +0O14 k "
int(12288)

# " +0o14 M"
int(12582912)

# " +0O14 M"
int(12582912)

# " +0o14 M "
int(12582912)

# " +0O14 M "
int(12582912)

# " +0o14 m"
int(12582912)

# " +0O14 m"
int(12582912)

# " +0o14 m "
int(12582912)

# " +0O14 m "
int(12582912)

# " -0o14"
int(-12)

# " -0O14"
int(-12)

# " -0o14 "
int(-12)

# " -0O14 "
int(-12)

# " -0o14K"
int(-12288)

# " -0O14K"
int(-12288)

# " -0o14K "
int(-12288)

# " -0O14K "
int(-12288)

# " -0o14k"
int(-12288)

# " -0O14k"
int(-12288)

# " -0o14k "
int(-12288)

# " -0O14k "
int(-12288)

# " -0o14M"
int(-12582912)

# " -0O14M"
int(-12582912)

# " -0o14M "
int(-12582912)

# " -0O14M "
int(-12582912)

# " -0o14m"
int(-12582912)

# " -0O14m"
int(-12582912)

# " -0o14m "
int(-12582912)

# " -0O14m "
int(-12582912)

# " -0o14 "
int(-12)

# " -0O14 "
int(-12)

# " -0o14  "
int(-12)

# " -0O14  "
int(-12)

# " -0o14 K"
int(-12288)

# " -0O14 K"
int(-12288)

# " -0o14 K "
int(-12288)

# " -0O14 K "
int(-12288)

# " -0o14 k"
int(-12288)

# " -0O14 k"
int(-12288)

# " -0o14 k "
int(-12288)

# " -0O14 k "
int(-12288)

# " -0o14 M"
int(-12582912)

# " -0O14 M"
int(-12582912)

# " -0o14 M "
int(-12582912)

# " -0O14 M "
int(-12582912)

# " -0o14 m"
int(-12582912)

# " -0O14 m"
int(-12582912)

# " -0o14 m "
int(-12582912)

# " -0O14 m "
int(-12582912)
