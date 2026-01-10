--TEST--
PDO_pgsql: PHP 8.5 deprecations
--EXTENSIONS--
pdo_pgsql
--FILE--
<?php

var_dump(
    PDO::PGSQL_ATTR_DISABLE_PREPARES,
);

?>
--EXPECTF--
Deprecated: Constant PDO::PGSQL_ATTR_DISABLE_PREPARES is deprecated since 8.5, use Pdo\Pgsql::ATTR_DISABLE_PREPARES instead in %s on line %d
int(1000)
