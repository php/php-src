--TEST--
PDO_mysql: PHP 8.5 deprecations
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
if (!defined('Pdo\\Mysql::ATTR_SSL_VERIFY_SERVER_CERT')) {
    die('skip requires mysqlnd');
}
?>
--FILE--
<?php

var_dump(
     PDO::MYSQL_ATTR_USE_BUFFERED_QUERY,
     PDO::MYSQL_ATTR_LOCAL_INFILE,
     PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY,
     PDO::MYSQL_ATTR_INIT_COMMAND,
     PDO::MYSQL_ATTR_COMPRESS,
     PDO::MYSQL_ATTR_DIRECT_QUERY,
     PDO::MYSQL_ATTR_FOUND_ROWS,
     PDO::MYSQL_ATTR_IGNORE_SPACE,
     PDO::MYSQL_ATTR_SSL_KEY,
     PDO::MYSQL_ATTR_SSL_CERT,
     PDO::MYSQL_ATTR_SSL_CA,
     PDO::MYSQL_ATTR_SSL_CAPATH,
     PDO::MYSQL_ATTR_SSL_CIPHER,
     PDO::MYSQL_ATTR_SSL_VERIFY_SERVER_CERT,
     PDO::MYSQL_ATTR_SERVER_PUBLIC_KEY,
     PDO::MYSQL_ATTR_MULTI_STATEMENTS,
);

?>
--EXPECTF--
Deprecated: Constant PDO::MYSQL_ATTR_USE_BUFFERED_QUERY is deprecated since 8.5, use Pdo\Mysql::ATTR_USE_BUFFERED_QUERY instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_LOCAL_INFILE is deprecated since 8.5, use Pdo\Mysql::ATTR_LOCAL_INFILE instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY is deprecated since 8.5, use Pdo\Mysql::ATTR_LOCAL_INFILE_DIRECTORY instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_INIT_COMMAND is deprecated since 8.5, use Pdo\Mysql::ATTR_INIT_COMMAND instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_COMPRESS is deprecated since 8.5, use Pdo\Mysql::ATTR_COMPRESS instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_DIRECT_QUERY is deprecated since 8.5, use Pdo\Mysql::ATTR_DIRECT_QUERY instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_FOUND_ROWS is deprecated since 8.5, use Pdo\Mysql::ATTR_FOUND_ROWS instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_IGNORE_SPACE is deprecated since 8.5, use Pdo\Mysql::ATTR_IGNORE_SPACE instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SSL_KEY is deprecated since 8.5, use Pdo\Mysql::ATTR_SSL_KEY instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SSL_CERT is deprecated since 8.5, use Pdo\Mysql::ATTR_SSL_CERT instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SSL_CA is deprecated since 8.5, use Pdo\Mysql::ATTR_SSL_CA instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SSL_CAPATH is deprecated since 8.5, use Pdo\Mysql::ATTR_SSL_CAPATH instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SSL_CIPHER is deprecated since 8.5, use Pdo\Mysql::ATTR_SSL_CIPHER instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SSL_VERIFY_SERVER_CERT is deprecated since 8.5, use Pdo\Mysql::ATTR_SSL_VERIFY_SERVER_CERT instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_SERVER_PUBLIC_KEY is deprecated since 8.5, use Pdo\Mysql::ATTR_SERVER_PUBLIC_KEY instead in %s on line %d

Deprecated: Constant PDO::MYSQL_ATTR_MULTI_STATEMENTS is deprecated since 8.5, use Pdo\Mysql::ATTR_MULTI_STATEMENTS instead in %s on line %d
int(1000)
int(1001)
int(1014)
int(1002)
int(1003)
int(20)
int(1004)
int(1005)
int(1006)
int(1007)
int(1008)
int(1009)
int(1010)
int(1013)
int(1011)
int(1012)
