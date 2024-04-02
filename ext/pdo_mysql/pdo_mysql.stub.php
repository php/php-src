<?php

/** @generate-class-entries */

/**
 * @strict-properties
 * @not-serializable
 */
class PdoMysql extends PDO
{
    /** @cvalue PDO_MYSQL_ATTR_USE_BUFFERED_QUERY */
    public const int ATTR_USE_BUFFERED_QUERY = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_LOCAL_INFILE */
    public const int ATTR_LOCAL_INFILE = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_INIT_COMMAND */
    public const int ATTR_INIT_COMMAND = UNKNOWN;

#ifndef PDO_USE_MYSQLND
    /** @cvalue PDO_MYSQL_ATTR_MAX_BUFFER_SIZE */
    public const int ATTR_MAX_BUFFER_SIZE = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_READ_DEFAULT_FILE */
    public const int ATTR_READ_DEFAULT_FILE = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_READ_DEFAULT_GROUP */
    public const int ATTR_READ_DEFAULT_GROUP = UNKNOWN;
#endif

    /** @cvalue PDO_MYSQL_ATTR_COMPRESS */
    public const int ATTR_COMPRESS = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_DIRECT_QUERY */
    public const int ATTR_DIRECT_QUERY = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_FOUND_ROWS */
    public const int ATTR_FOUND_ROWS = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_IGNORE_SPACE */
    public const int ATTR_IGNORE_SPACE = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_SSL_KEY */
    public const int ATTR_SSL_KEY = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_SSL_CERT */
    public const int ATTR_SSL_CERT = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_SSL_CA */
    public const int ATTR_SSL_CA = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_SSL_CAPATH */
    public const int ATTR_SSL_CAPATH = UNKNOWN;

    /** @cvalue PDO_MYSQL_ATTR_SSL_CIPHER */
    public const int ATTR_SSL_CIPHER = UNKNOWN;

#if MYSQL_VERSION_ID > 50605 || defined(PDO_USE_MYSQLND)
    /** @cvalue PDO_MYSQL_ATTR_SERVER_PUBLIC_KEY */
    public const int ATTR_SERVER_PUBLIC_KEY = UNKNOWN;
#endif

    /** @cvalue PDO_MYSQL_ATTR_MULTI_STATEMENTS */
    public const int ATTR_MULTI_STATEMENTS = UNKNOWN;
#ifdef PDO_USE_MYSQLND
    /** @cvalue PDO_MYSQL_ATTR_SSL_VERIFY_SERVER_CERT */
    public const int ATTR_SSL_VERIFY_SERVER_CERT = UNKNOWN;
#endif
#if MYSQL_VERSION_ID >= 80021 || defined(PDO_USE_MYSQLND)
    /** @cvalue PDO_MYSQL_ATTR_LOCAL_INFILE_DIRECTORY */
    public const int ATTR_LOCAL_INFILE_DIRECTORY = UNKNOWN;
#endif

    public function getWarningCount(): int {}
}
