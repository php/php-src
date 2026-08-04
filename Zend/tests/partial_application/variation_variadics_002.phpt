--TEST--
PFA variation: variadics, internal function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--FILE--
<?php
$sprintf = sprintf("%d %d %d", 100, ...);

echo (string) new ReflectionFunction($sprintf);

echo $sprintf(1000, 10000);
?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %svariation_variadics_002.php 2 - 2

  - Parameters [1] {
    Parameter #0 [ <optional> mixed ...$values ]
  }
  - Return [ string ]
}
100 1000 10000
