--TEST--
Test parsing of quantities (e.g. "16M")
--EXTENSIONS--
zend_test
--FILE--
<?php

// This test checks valid formats do not throw any warnings.
foreach (['', ' '] as $leadingWS) {
  foreach (['', '+', '-'] as $sign) {
    foreach (['', ' '] as $midWS) {
      foreach (['', 'K', 'k', 'M', 'm', 'G', 'g'] as $exp) {
        foreach (['', ' '] as $trailingWS) {
          $setting = sprintf('%s%s1%s%s%s',
                             $leadingWS, $sign, $midWS, $exp, $trailingWS);
          var_dump($setting);
          var_dump(zend_test_zend_ini_parse_quantity($setting));
        }
      }
    }
  }
}

--EXPECT--
string(1) "1"
int(1)
string(2) "1 "
int(1)
string(2) "1K"
int(1024)
string(3) "1K "
int(1024)
string(2) "1k"
int(1024)
string(3) "1k "
int(1024)
string(2) "1M"
int(1048576)
string(3) "1M "
int(1048576)
string(2) "1m"
int(1048576)
string(3) "1m "
int(1048576)
string(2) "1G"
int(1073741824)
string(3) "1G "
int(1073741824)
string(2) "1g"
int(1073741824)
string(3) "1g "
int(1073741824)
string(2) "1 "
int(1)
string(3) "1  "
int(1)
string(3) "1 K"
int(1024)
string(4) "1 K "
int(1024)
string(3) "1 k"
int(1024)
string(4) "1 k "
int(1024)
string(3) "1 M"
int(1048576)
string(4) "1 M "
int(1048576)
string(3) "1 m"
int(1048576)
string(4) "1 m "
int(1048576)
string(3) "1 G"
int(1073741824)
string(4) "1 G "
int(1073741824)
string(3) "1 g"
int(1073741824)
string(4) "1 g "
int(1073741824)
string(2) "+1"
int(1)
string(3) "+1 "
int(1)
string(3) "+1K"
int(1024)
string(4) "+1K "
int(1024)
string(3) "+1k"
int(1024)
string(4) "+1k "
int(1024)
string(3) "+1M"
int(1048576)
string(4) "+1M "
int(1048576)
string(3) "+1m"
int(1048576)
string(4) "+1m "
int(1048576)
string(3) "+1G"
int(1073741824)
string(4) "+1G "
int(1073741824)
string(3) "+1g"
int(1073741824)
string(4) "+1g "
int(1073741824)
string(3) "+1 "
int(1)
string(4) "+1  "
int(1)
string(4) "+1 K"
int(1024)
string(5) "+1 K "
int(1024)
string(4) "+1 k"
int(1024)
string(5) "+1 k "
int(1024)
string(4) "+1 M"
int(1048576)
string(5) "+1 M "
int(1048576)
string(4) "+1 m"
int(1048576)
string(5) "+1 m "
int(1048576)
string(4) "+1 G"
int(1073741824)
string(5) "+1 G "
int(1073741824)
string(4) "+1 g"
int(1073741824)
string(5) "+1 g "
int(1073741824)
string(2) "-1"
int(-1)
string(3) "-1 "
int(-1)
string(3) "-1K"
int(-1024)
string(4) "-1K "
int(-1024)
string(3) "-1k"
int(-1024)
string(4) "-1k "
int(-1024)
string(3) "-1M"
int(-1048576)
string(4) "-1M "
int(-1048576)
string(3) "-1m"
int(-1048576)
string(4) "-1m "
int(-1048576)
string(3) "-1G"
int(-1073741824)
string(4) "-1G "
int(-1073741824)
string(3) "-1g"
int(-1073741824)
string(4) "-1g "
int(-1073741824)
string(3) "-1 "
int(-1)
string(4) "-1  "
int(-1)
string(4) "-1 K"
int(-1024)
string(5) "-1 K "
int(-1024)
string(4) "-1 k"
int(-1024)
string(5) "-1 k "
int(-1024)
string(4) "-1 M"
int(-1048576)
string(5) "-1 M "
int(-1048576)
string(4) "-1 m"
int(-1048576)
string(5) "-1 m "
int(-1048576)
string(4) "-1 G"
int(-1073741824)
string(5) "-1 G "
int(-1073741824)
string(4) "-1 g"
int(-1073741824)
string(5) "-1 g "
int(-1073741824)
string(2) " 1"
int(1)
string(3) " 1 "
int(1)
string(3) " 1K"
int(1024)
string(4) " 1K "
int(1024)
string(3) " 1k"
int(1024)
string(4) " 1k "
int(1024)
string(3) " 1M"
int(1048576)
string(4) " 1M "
int(1048576)
string(3) " 1m"
int(1048576)
string(4) " 1m "
int(1048576)
string(3) " 1G"
int(1073741824)
string(4) " 1G "
int(1073741824)
string(3) " 1g"
int(1073741824)
string(4) " 1g "
int(1073741824)
string(3) " 1 "
int(1)
string(4) " 1  "
int(1)
string(4) " 1 K"
int(1024)
string(5) " 1 K "
int(1024)
string(4) " 1 k"
int(1024)
string(5) " 1 k "
int(1024)
string(4) " 1 M"
int(1048576)
string(5) " 1 M "
int(1048576)
string(4) " 1 m"
int(1048576)
string(5) " 1 m "
int(1048576)
string(4) " 1 G"
int(1073741824)
string(5) " 1 G "
int(1073741824)
string(4) " 1 g"
int(1073741824)
string(5) " 1 g "
int(1073741824)
string(3) " +1"
int(1)
string(4) " +1 "
int(1)
string(4) " +1K"
int(1024)
string(5) " +1K "
int(1024)
string(4) " +1k"
int(1024)
string(5) " +1k "
int(1024)
string(4) " +1M"
int(1048576)
string(5) " +1M "
int(1048576)
string(4) " +1m"
int(1048576)
string(5) " +1m "
int(1048576)
string(4) " +1G"
int(1073741824)
string(5) " +1G "
int(1073741824)
string(4) " +1g"
int(1073741824)
string(5) " +1g "
int(1073741824)
string(4) " +1 "
int(1)
string(5) " +1  "
int(1)
string(5) " +1 K"
int(1024)
string(6) " +1 K "
int(1024)
string(5) " +1 k"
int(1024)
string(6) " +1 k "
int(1024)
string(5) " +1 M"
int(1048576)
string(6) " +1 M "
int(1048576)
string(5) " +1 m"
int(1048576)
string(6) " +1 m "
int(1048576)
string(5) " +1 G"
int(1073741824)
string(6) " +1 G "
int(1073741824)
string(5) " +1 g"
int(1073741824)
string(6) " +1 g "
int(1073741824)
string(3) " -1"
int(-1)
string(4) " -1 "
int(-1)
string(4) " -1K"
int(-1024)
string(5) " -1K "
int(-1024)
string(4) " -1k"
int(-1024)
string(5) " -1k "
int(-1024)
string(4) " -1M"
int(-1048576)
string(5) " -1M "
int(-1048576)
string(4) " -1m"
int(-1048576)
string(5) " -1m "
int(-1048576)
string(4) " -1G"
int(-1073741824)
string(5) " -1G "
int(-1073741824)
string(4) " -1g"
int(-1073741824)
string(5) " -1g "
int(-1073741824)
string(4) " -1 "
int(-1)
string(5) " -1  "
int(-1)
string(5) " -1 K"
int(-1024)
string(6) " -1 K "
int(-1024)
string(5) " -1 k"
int(-1024)
string(6) " -1 k "
int(-1024)
string(5) " -1 M"
int(-1048576)
string(6) " -1 M "
int(-1048576)
string(5) " -1 m"
int(-1048576)
string(6) " -1 m "
int(-1048576)
string(5) " -1 G"
int(-1073741824)
string(6) " -1 G "
int(-1073741824)
string(5) " -1 g"
int(-1073741824)
string(6) " -1 g "
int(-1073741824)
