--TEST--
Test parsing of hexadecimal quantities
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
          $setting = sprintf('%s%s0x1F%s%s%s',
                             $leadingWS, $sign, $midWS, $exp, $trailingWS);
          printf("# \"%s\"\n", $setting);
          var_dump(zend_test_zend_ini_parse_quantity($setting));
          print "\n";

          $setting = sprintf('%s%s0X1F%s%s%s',
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
# "0x1F"
int(31)

# "0X1F"
int(31)

# "0x1F "
int(31)

# "0X1F "
int(31)

# "0x1FK"
int(31744)

# "0X1FK"
int(31744)

# "0x1FK "
int(31744)

# "0X1FK "
int(31744)

# "0x1Fk"
int(31744)

# "0X1Fk"
int(31744)

# "0x1Fk "
int(31744)

# "0X1Fk "
int(31744)

# "0x1FM"
int(32505856)

# "0X1FM"
int(32505856)

# "0x1FM "
int(32505856)

# "0X1FM "
int(32505856)

# "0x1Fm"
int(32505856)

# "0X1Fm"
int(32505856)

# "0x1Fm "
int(32505856)

# "0X1Fm "
int(32505856)

# "0x1F "
int(31)

# "0X1F "
int(31)

# "0x1F  "
int(31)

# "0X1F  "
int(31)

# "0x1F K"
int(31744)

# "0X1F K"
int(31744)

# "0x1F K "
int(31744)

# "0X1F K "
int(31744)

# "0x1F k"
int(31744)

# "0X1F k"
int(31744)

# "0x1F k "
int(31744)

# "0X1F k "
int(31744)

# "0x1F M"
int(32505856)

# "0X1F M"
int(32505856)

# "0x1F M "
int(32505856)

# "0X1F M "
int(32505856)

# "0x1F m"
int(32505856)

# "0X1F m"
int(32505856)

# "0x1F m "
int(32505856)

# "0X1F m "
int(32505856)

# "+0x1F"
int(31)

# "+0X1F"
int(31)

# "+0x1F "
int(31)

# "+0X1F "
int(31)

# "+0x1FK"
int(31744)

# "+0X1FK"
int(31744)

# "+0x1FK "
int(31744)

# "+0X1FK "
int(31744)

# "+0x1Fk"
int(31744)

# "+0X1Fk"
int(31744)

# "+0x1Fk "
int(31744)

# "+0X1Fk "
int(31744)

# "+0x1FM"
int(32505856)

# "+0X1FM"
int(32505856)

# "+0x1FM "
int(32505856)

# "+0X1FM "
int(32505856)

# "+0x1Fm"
int(32505856)

# "+0X1Fm"
int(32505856)

# "+0x1Fm "
int(32505856)

# "+0X1Fm "
int(32505856)

# "+0x1F "
int(31)

# "+0X1F "
int(31)

# "+0x1F  "
int(31)

# "+0X1F  "
int(31)

# "+0x1F K"
int(31744)

# "+0X1F K"
int(31744)

# "+0x1F K "
int(31744)

# "+0X1F K "
int(31744)

# "+0x1F k"
int(31744)

# "+0X1F k"
int(31744)

# "+0x1F k "
int(31744)

# "+0X1F k "
int(31744)

# "+0x1F M"
int(32505856)

# "+0X1F M"
int(32505856)

# "+0x1F M "
int(32505856)

# "+0X1F M "
int(32505856)

# "+0x1F m"
int(32505856)

# "+0X1F m"
int(32505856)

# "+0x1F m "
int(32505856)

# "+0X1F m "
int(32505856)

# "-0x1F"
int(-31)

# "-0X1F"
int(-31)

# "-0x1F "
int(-31)

# "-0X1F "
int(-31)

# "-0x1FK"
int(-31744)

# "-0X1FK"
int(-31744)

# "-0x1FK "
int(-31744)

# "-0X1FK "
int(-31744)

# "-0x1Fk"
int(-31744)

# "-0X1Fk"
int(-31744)

# "-0x1Fk "
int(-31744)

# "-0X1Fk "
int(-31744)

# "-0x1FM"
int(-32505856)

# "-0X1FM"
int(-32505856)

# "-0x1FM "
int(-32505856)

# "-0X1FM "
int(-32505856)

# "-0x1Fm"
int(-32505856)

# "-0X1Fm"
int(-32505856)

# "-0x1Fm "
int(-32505856)

# "-0X1Fm "
int(-32505856)

# "-0x1F "
int(-31)

# "-0X1F "
int(-31)

# "-0x1F  "
int(-31)

# "-0X1F  "
int(-31)

# "-0x1F K"
int(-31744)

# "-0X1F K"
int(-31744)

# "-0x1F K "
int(-31744)

# "-0X1F K "
int(-31744)

# "-0x1F k"
int(-31744)

# "-0X1F k"
int(-31744)

# "-0x1F k "
int(-31744)

# "-0X1F k "
int(-31744)

# "-0x1F M"
int(-32505856)

# "-0X1F M"
int(-32505856)

# "-0x1F M "
int(-32505856)

# "-0X1F M "
int(-32505856)

# "-0x1F m"
int(-32505856)

# "-0X1F m"
int(-32505856)

# "-0x1F m "
int(-32505856)

# "-0X1F m "
int(-32505856)

# " 0x1F"
int(31)

# " 0X1F"
int(31)

# " 0x1F "
int(31)

# " 0X1F "
int(31)

# " 0x1FK"
int(31744)

# " 0X1FK"
int(31744)

# " 0x1FK "
int(31744)

# " 0X1FK "
int(31744)

# " 0x1Fk"
int(31744)

# " 0X1Fk"
int(31744)

# " 0x1Fk "
int(31744)

# " 0X1Fk "
int(31744)

# " 0x1FM"
int(32505856)

# " 0X1FM"
int(32505856)

# " 0x1FM "
int(32505856)

# " 0X1FM "
int(32505856)

# " 0x1Fm"
int(32505856)

# " 0X1Fm"
int(32505856)

# " 0x1Fm "
int(32505856)

# " 0X1Fm "
int(32505856)

# " 0x1F "
int(31)

# " 0X1F "
int(31)

# " 0x1F  "
int(31)

# " 0X1F  "
int(31)

# " 0x1F K"
int(31744)

# " 0X1F K"
int(31744)

# " 0x1F K "
int(31744)

# " 0X1F K "
int(31744)

# " 0x1F k"
int(31744)

# " 0X1F k"
int(31744)

# " 0x1F k "
int(31744)

# " 0X1F k "
int(31744)

# " 0x1F M"
int(32505856)

# " 0X1F M"
int(32505856)

# " 0x1F M "
int(32505856)

# " 0X1F M "
int(32505856)

# " 0x1F m"
int(32505856)

# " 0X1F m"
int(32505856)

# " 0x1F m "
int(32505856)

# " 0X1F m "
int(32505856)

# " +0x1F"
int(31)

# " +0X1F"
int(31)

# " +0x1F "
int(31)

# " +0X1F "
int(31)

# " +0x1FK"
int(31744)

# " +0X1FK"
int(31744)

# " +0x1FK "
int(31744)

# " +0X1FK "
int(31744)

# " +0x1Fk"
int(31744)

# " +0X1Fk"
int(31744)

# " +0x1Fk "
int(31744)

# " +0X1Fk "
int(31744)

# " +0x1FM"
int(32505856)

# " +0X1FM"
int(32505856)

# " +0x1FM "
int(32505856)

# " +0X1FM "
int(32505856)

# " +0x1Fm"
int(32505856)

# " +0X1Fm"
int(32505856)

# " +0x1Fm "
int(32505856)

# " +0X1Fm "
int(32505856)

# " +0x1F "
int(31)

# " +0X1F "
int(31)

# " +0x1F  "
int(31)

# " +0X1F  "
int(31)

# " +0x1F K"
int(31744)

# " +0X1F K"
int(31744)

# " +0x1F K "
int(31744)

# " +0X1F K "
int(31744)

# " +0x1F k"
int(31744)

# " +0X1F k"
int(31744)

# " +0x1F k "
int(31744)

# " +0X1F k "
int(31744)

# " +0x1F M"
int(32505856)

# " +0X1F M"
int(32505856)

# " +0x1F M "
int(32505856)

# " +0X1F M "
int(32505856)

# " +0x1F m"
int(32505856)

# " +0X1F m"
int(32505856)

# " +0x1F m "
int(32505856)

# " +0X1F m "
int(32505856)

# " -0x1F"
int(-31)

# " -0X1F"
int(-31)

# " -0x1F "
int(-31)

# " -0X1F "
int(-31)

# " -0x1FK"
int(-31744)

# " -0X1FK"
int(-31744)

# " -0x1FK "
int(-31744)

# " -0X1FK "
int(-31744)

# " -0x1Fk"
int(-31744)

# " -0X1Fk"
int(-31744)

# " -0x1Fk "
int(-31744)

# " -0X1Fk "
int(-31744)

# " -0x1FM"
int(-32505856)

# " -0X1FM"
int(-32505856)

# " -0x1FM "
int(-32505856)

# " -0X1FM "
int(-32505856)

# " -0x1Fm"
int(-32505856)

# " -0X1Fm"
int(-32505856)

# " -0x1Fm "
int(-32505856)

# " -0X1Fm "
int(-32505856)

# " -0x1F "
int(-31)

# " -0X1F "
int(-31)

# " -0x1F  "
int(-31)

# " -0X1F  "
int(-31)

# " -0x1F K"
int(-31744)

# " -0X1F K"
int(-31744)

# " -0x1F K "
int(-31744)

# " -0X1F K "
int(-31744)

# " -0x1F k"
int(-31744)

# " -0X1F k"
int(-31744)

# " -0x1F k "
int(-31744)

# " -0X1F k "
int(-31744)

# " -0x1F M"
int(-32505856)

# " -0X1F M"
int(-32505856)

# " -0x1F M "
int(-32505856)

# " -0X1F M "
int(-32505856)

# " -0x1F m"
int(-32505856)

# " -0X1F m"
int(-32505856)

# " -0x1F m "
int(-32505856)

# " -0X1F m "
int(-32505856)
