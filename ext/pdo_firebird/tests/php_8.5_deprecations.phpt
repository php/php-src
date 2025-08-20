--TEST--
PDO_firebird: PHP 8.5 deprecations
--EXTENSIONS--
pdo_firebird
--FILE--
<?php

var_dump(
    PDO::FB_ATTR_DATE_FORMAT,
    PDO::FB_ATTR_TIME_FORMAT,
    PDO::FB_ATTR_TIMESTAMP_FORMAT,
);

?>
--EXPECTF--
Deprecated: Constant PDO::FB_ATTR_DATE_FORMAT is deprecated since 8.5, use Pdo\Firebird::ATTR_DATE_FORMAT instead in %s on line %d

Deprecated: Constant PDO::FB_ATTR_TIME_FORMAT is deprecated since 8.5, use Pdo\Firebird::ATTR_TIME_FORMAT instead in %s on line %d

Deprecated: Constant PDO::FB_ATTR_TIMESTAMP_FORMAT is deprecated since 8.5, use Pdo\Firebird::ATTR_TIMESTAMP_FORMAT instead in %s on line %d
int(1000)
int(1001)
int(1002)
