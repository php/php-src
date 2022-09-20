--TEST--
Test parsing of binary quantities
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
          $setting = sprintf('%s%s0b11%s%s%s',
                             $leadingWS, $sign, $midWS, $exp, $trailingWS);
          printf("# \"%s\"\n", $setting);
          var_dump(zend_test_zend_ini_parse_quantity($setting));
          print "\n";

          $setting = sprintf('%s%s0B11%s%s%s',
                             $leadingWS, $sign, $midWS, $exp, $trailingWS);
          printf("# \"%s\"\n", $setting);
          var_dump(zend_test_zend_ini_parse_quantity($setting));
          print "\n";
        }
      }
    }
  }
}
--EXPECT--
# "0b11"
int(3)

# "0B11"
int(3)

# "0b11 "
int(3)

# "0B11 "
int(3)

# "0b11K"
int(3072)

# "0B11K"
int(3072)

# "0b11K "
int(3072)

# "0B11K "
int(3072)

# "0b11k"
int(3072)

# "0B11k"
int(3072)

# "0b11k "
int(3072)

# "0B11k "
int(3072)

# "0b11M"
int(3145728)

# "0B11M"
int(3145728)

# "0b11M "
int(3145728)

# "0B11M "
int(3145728)

# "0b11m"
int(3145728)

# "0B11m"
int(3145728)

# "0b11m "
int(3145728)

# "0B11m "
int(3145728)

# "0b11 "
int(3)

# "0B11 "
int(3)

# "0b11  "
int(3)

# "0B11  "
int(3)

# "0b11 K"
int(3072)

# "0B11 K"
int(3072)

# "0b11 K "
int(3072)

# "0B11 K "
int(3072)

# "0b11 k"
int(3072)

# "0B11 k"
int(3072)

# "0b11 k "
int(3072)

# "0B11 k "
int(3072)

# "0b11 M"
int(3145728)

# "0B11 M"
int(3145728)

# "0b11 M "
int(3145728)

# "0B11 M "
int(3145728)

# "0b11 m"
int(3145728)

# "0B11 m"
int(3145728)

# "0b11 m "
int(3145728)

# "0B11 m "
int(3145728)

# "+0b11"
int(3)

# "+0B11"
int(3)

# "+0b11 "
int(3)

# "+0B11 "
int(3)

# "+0b11K"
int(3072)

# "+0B11K"
int(3072)

# "+0b11K "
int(3072)

# "+0B11K "
int(3072)

# "+0b11k"
int(3072)

# "+0B11k"
int(3072)

# "+0b11k "
int(3072)

# "+0B11k "
int(3072)

# "+0b11M"
int(3145728)

# "+0B11M"
int(3145728)

# "+0b11M "
int(3145728)

# "+0B11M "
int(3145728)

# "+0b11m"
int(3145728)

# "+0B11m"
int(3145728)

# "+0b11m "
int(3145728)

# "+0B11m "
int(3145728)

# "+0b11 "
int(3)

# "+0B11 "
int(3)

# "+0b11  "
int(3)

# "+0B11  "
int(3)

# "+0b11 K"
int(3072)

# "+0B11 K"
int(3072)

# "+0b11 K "
int(3072)

# "+0B11 K "
int(3072)

# "+0b11 k"
int(3072)

# "+0B11 k"
int(3072)

# "+0b11 k "
int(3072)

# "+0B11 k "
int(3072)

# "+0b11 M"
int(3145728)

# "+0B11 M"
int(3145728)

# "+0b11 M "
int(3145728)

# "+0B11 M "
int(3145728)

# "+0b11 m"
int(3145728)

# "+0B11 m"
int(3145728)

# "+0b11 m "
int(3145728)

# "+0B11 m "
int(3145728)

# "-0b11"
int(-3)

# "-0B11"
int(-3)

# "-0b11 "
int(-3)

# "-0B11 "
int(-3)

# "-0b11K"
int(-3072)

# "-0B11K"
int(-3072)

# "-0b11K "
int(-3072)

# "-0B11K "
int(-3072)

# "-0b11k"
int(-3072)

# "-0B11k"
int(-3072)

# "-0b11k "
int(-3072)

# "-0B11k "
int(-3072)

# "-0b11M"
int(-3145728)

# "-0B11M"
int(-3145728)

# "-0b11M "
int(-3145728)

# "-0B11M "
int(-3145728)

# "-0b11m"
int(-3145728)

# "-0B11m"
int(-3145728)

# "-0b11m "
int(-3145728)

# "-0B11m "
int(-3145728)

# "-0b11 "
int(-3)

# "-0B11 "
int(-3)

# "-0b11  "
int(-3)

# "-0B11  "
int(-3)

# "-0b11 K"
int(-3072)

# "-0B11 K"
int(-3072)

# "-0b11 K "
int(-3072)

# "-0B11 K "
int(-3072)

# "-0b11 k"
int(-3072)

# "-0B11 k"
int(-3072)

# "-0b11 k "
int(-3072)

# "-0B11 k "
int(-3072)

# "-0b11 M"
int(-3145728)

# "-0B11 M"
int(-3145728)

# "-0b11 M "
int(-3145728)

# "-0B11 M "
int(-3145728)

# "-0b11 m"
int(-3145728)

# "-0B11 m"
int(-3145728)

# "-0b11 m "
int(-3145728)

# "-0B11 m "
int(-3145728)

# " 0b11"
int(3)

# " 0B11"
int(3)

# " 0b11 "
int(3)

# " 0B11 "
int(3)

# " 0b11K"
int(3072)

# " 0B11K"
int(3072)

# " 0b11K "
int(3072)

# " 0B11K "
int(3072)

# " 0b11k"
int(3072)

# " 0B11k"
int(3072)

# " 0b11k "
int(3072)

# " 0B11k "
int(3072)

# " 0b11M"
int(3145728)

# " 0B11M"
int(3145728)

# " 0b11M "
int(3145728)

# " 0B11M "
int(3145728)

# " 0b11m"
int(3145728)

# " 0B11m"
int(3145728)

# " 0b11m "
int(3145728)

# " 0B11m "
int(3145728)

# " 0b11 "
int(3)

# " 0B11 "
int(3)

# " 0b11  "
int(3)

# " 0B11  "
int(3)

# " 0b11 K"
int(3072)

# " 0B11 K"
int(3072)

# " 0b11 K "
int(3072)

# " 0B11 K "
int(3072)

# " 0b11 k"
int(3072)

# " 0B11 k"
int(3072)

# " 0b11 k "
int(3072)

# " 0B11 k "
int(3072)

# " 0b11 M"
int(3145728)

# " 0B11 M"
int(3145728)

# " 0b11 M "
int(3145728)

# " 0B11 M "
int(3145728)

# " 0b11 m"
int(3145728)

# " 0B11 m"
int(3145728)

# " 0b11 m "
int(3145728)

# " 0B11 m "
int(3145728)

# " +0b11"
int(3)

# " +0B11"
int(3)

# " +0b11 "
int(3)

# " +0B11 "
int(3)

# " +0b11K"
int(3072)

# " +0B11K"
int(3072)

# " +0b11K "
int(3072)

# " +0B11K "
int(3072)

# " +0b11k"
int(3072)

# " +0B11k"
int(3072)

# " +0b11k "
int(3072)

# " +0B11k "
int(3072)

# " +0b11M"
int(3145728)

# " +0B11M"
int(3145728)

# " +0b11M "
int(3145728)

# " +0B11M "
int(3145728)

# " +0b11m"
int(3145728)

# " +0B11m"
int(3145728)

# " +0b11m "
int(3145728)

# " +0B11m "
int(3145728)

# " +0b11 "
int(3)

# " +0B11 "
int(3)

# " +0b11  "
int(3)

# " +0B11  "
int(3)

# " +0b11 K"
int(3072)

# " +0B11 K"
int(3072)

# " +0b11 K "
int(3072)

# " +0B11 K "
int(3072)

# " +0b11 k"
int(3072)

# " +0B11 k"
int(3072)

# " +0b11 k "
int(3072)

# " +0B11 k "
int(3072)

# " +0b11 M"
int(3145728)

# " +0B11 M"
int(3145728)

# " +0b11 M "
int(3145728)

# " +0B11 M "
int(3145728)

# " +0b11 m"
int(3145728)

# " +0B11 m"
int(3145728)

# " +0b11 m "
int(3145728)

# " +0B11 m "
int(3145728)

# " -0b11"
int(-3)

# " -0B11"
int(-3)

# " -0b11 "
int(-3)

# " -0B11 "
int(-3)

# " -0b11K"
int(-3072)

# " -0B11K"
int(-3072)

# " -0b11K "
int(-3072)

# " -0B11K "
int(-3072)

# " -0b11k"
int(-3072)

# " -0B11k"
int(-3072)

# " -0b11k "
int(-3072)

# " -0B11k "
int(-3072)

# " -0b11M"
int(-3145728)

# " -0B11M"
int(-3145728)

# " -0b11M "
int(-3145728)

# " -0B11M "
int(-3145728)

# " -0b11m"
int(-3145728)

# " -0B11m"
int(-3145728)

# " -0b11m "
int(-3145728)

# " -0B11m "
int(-3145728)

# " -0b11 "
int(-3)

# " -0B11 "
int(-3)

# " -0b11  "
int(-3)

# " -0B11  "
int(-3)

# " -0b11 K"
int(-3072)

# " -0B11 K"
int(-3072)

# " -0b11 K "
int(-3072)

# " -0B11 K "
int(-3072)

# " -0b11 k"
int(-3072)

# " -0B11 k"
int(-3072)

# " -0b11 k "
int(-3072)

# " -0B11 k "
int(-3072)

# " -0b11 M"
int(-3145728)

# " -0B11 M"
int(-3145728)

# " -0b11 M "
int(-3145728)

# " -0B11 M "
int(-3145728)

# " -0b11 m"
int(-3145728)

# " -0B11 m"
int(-3145728)

# " -0b11 m "
int(-3145728)

# " -0B11 m "
int(-3145728)
