<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$
//
// Database independent query interface.
//

require_once "PEAR.php";

/*
 * The method mapErrorCode in each DB_dbtype implementation maps
 * native error codes to one of these.
 *
 * If you add an error code here, make sure you also add a textual
 * version of it in DB::errorMessage().
 */

define("DB_OK",                         1);
define("DB_ERROR",                     -1);
define("DB_ERROR_SYNTAX",              -2);
define("DB_ERROR_CONSTRAINT",          -3);
define("DB_ERROR_NOT_FOUND",           -4);
define("DB_ERROR_ALREADY_EXISTS",      -5);
define("DB_ERROR_UNSUPPORTED",         -6);
define("DB_ERROR_MISMATCH",            -7);
define("DB_ERROR_INVALID",             -8);
define("DB_ERROR_NOT_CAPABLE",         -9);
define("DB_ERROR_TRUNCATED",          -10);
define("DB_ERROR_INVALID_NUMBER",     -11);
define("DB_ERROR_INVALID_DATE",       -12);
define("DB_ERROR_DIVZERO",            -13);
define("DB_ERROR_NODBSELECTED",       -14);
define("DB_ERROR_CANNOT_CREATE",      -15);
define("DB_ERROR_CANNOT_DELETE",      -16);
define("DB_ERROR_CANNOT_DROP",        -17);
define("DB_ERROR_NOSUCHTABLE",        -18);
define("DB_ERROR_NOSUCHFIELD",        -19);
define("DB_ERROR_NEED_MORE_DATA",     -20);
define("DB_ERROR_NOT_LOCKED",         -21);
define("DB_ERROR_VALUE_COUNT_ON_ROW", -22);
define("DB_ERROR_INVALID_DSN",        -23);
define("DB_ERROR_CONNECT_FAILED",     -24);

/*
 * Warnings are not detected as errors by DB::isError(), and are not
 * fatal.  You can detect whether an error is in fact a warning with
 * DB::isWarning().
 */

define('DB_WARNING',           -1000);
define('DB_WARNING_READ_ONLY', -1001);

/*
 * These constants are used when storing information about prepared
 * statements (using the "prepare" method in DB_dbtype).
 *
 * The prepare/execute model in DB is mostly borrowed from the ODBC
 * extension, in a query the "?" character means a scalar parameter.
 * There is one extension though, a "*" character means an opaque
 * parameter.  An opaque parameter is simply a file name, the real
 * data are in that file (useful for stuff like putting uploaded files
 * into your database).
 */

define('DB_PARAM_SCALAR', 1);
define('DB_PARAM_OPAQUE', 2);

/*
 * These constants define different ways of returning binary data
 * from queries.  Again, this model has been borrowed from the ODBC
 * extension.
 *
 * DB_BINMODE_PASSTHRU sends the data directly through to the browser
 * when data is fetched from the database.
 * DB_BINMODE_RETURN lets you return data as usual.
 * DB_BINMODE_CONVERT returns data as well, only it is converted to
 * hex format, for example the string "123" would become "313233".
 */

define('DB_BINMODE_PASSTHRU', 1);
define('DB_BINMODE_RETURN',   2);
define('DB_BINMODE_CONVERT',  3);

/**
 * This is a special constant that tells DB the user hasn't specified
 * any particular get mode, so the default should be used.
 */

define('DB_FETCHMODE_DEFAULT', 0);

/**
 * Column data indexed by numbers, ordered from 0 and up
 */

define('DB_FETCHMODE_ORDERED', 1);

/**
 * Column data indexed by column names
 */

define('DB_FETCHMODE_ASSOC', 2);

/**
 * For multi-dimensional results: normally the first level of arrays
 * is the row number, and the second level indexed by column number or name.
 * DB_FETCHMODE_FLIPPED switches this order, so the first level of arrays
 * is the column name, and the second level the row number.
 */

define('DB_FETCHMODE_FLIPPED', 4);

/* for compatibility */

define('DB_GETMODE_ORDERED', DB_FETCHMODE_ORDERED);
define('DB_GETMODE_ASSOC',   DB_FETCHMODE_ASSOC);
define('DB_GETMODE_FLIPPED', DB_FETCHMODE_FLIPPED);

/**
 * these are constants for the tableInfo-function
 * they are bitwised or'ed. so if there are more constants to be defined
 * in the future, adjust DB_TABLEINFO_FULL accordingly
 */

define('DB_TABLEINFO_ORDER', 1);
define('DB_TABLEINFO_ORDERTABLE', 2);
define('DB_TABLEINFO_FULL', 3);


/**
 * The main "DB" class is simply a container class with some static
 * methods for creating DB objects as well as some utility functions
 * common to all parts of DB.
 *
 * The object model of DB is as follows (indentation means inheritance):
 *
 * DB           The main DB class.  This is simply a utility class
 *              with some "static" methods for creating DB objects as
 *              well as common utility functions for other DB classes.
 *
 * DB_common    The base for each DB implementation.  Provides default
 * |            implementations (in OO lingo virtual methods) for
 * |            the actual DB implementations as well as a bunch of
 * |            query utility functions.
 * |
 * +-DB_mysql   The DB implementation for MySQL.  Inherits DB_common.
 *              When calling DB::factory or DB::connect for MySQL
 *              connections, the object returned is an instance of this
 *              class.
 *
 * @version  2
 * @author   Stig Bakken <ssb@fast.no>
 * @since    PHP 4.0
 */

class DB
{
    /**
     * Create a new DB object for the specified database type
     *
     * @param $type string database type, for example "mysql"
     *
     * @return object a newly created DB object, or a DB error code on
     * error
     */

    function &factory($type)
    {
        @include_once("DB/${type}.php");

        $classname = "DB_${type}";

        if (!class_exists($classname)) {
            return PEAR::raiseError(null, DB_ERROR_NOT_FOUND,
                                    null, null, null, 'DB_Error', true);
        }

        @$obj =& new $classname;

        return $obj;
    }

    /**
     * Create a new DB object and connect to the specified database
     *
     * @param $dsn mixed "data source name", see the DB::parseDSN
     * method for a description of the dsn format.  Can also be
     * specified as an array of the format returned by DB::parseDSN.
     *
     * @param $options mixed if boolean (or scalar), tells whether
     * this connection should be persistent (for backends that support
     * this).  This parameter can also be an array of options, see
     * DB_common::setOption for more information on connection
     * options.
     *
     * @return object a newly created DB connection object, or a DB
     * error object on error
     *
     * @see DB::parseDSN
     * @see DB::isError
     */
    function &connect($dsn, $options = false)
    {
        if (is_array($dsn)) {
            $dsninfo = $dsn;
        } else {
            $dsninfo = DB::parseDSN($dsn);
        }
        $type = $dsninfo["phptype"];

        if (is_array($options) && isset($options["debug"]) &&
            $options["debug"] >= 2) {
            // expose php errors with sufficient debug level
            include_once "DB/${type}.php";
        } else {
            @include_once "DB/${type}.php";
        }

        $classname = "DB_${type}";
        if (!class_exists($classname)) {
            return PEAR::raiseError(null, DB_ERROR_NOT_FOUND,
                                    null, null, null, 'DB_Error', true);
        }

        @$obj =& new $classname;

        if (is_array($options)) {
            foreach ($options as $option => $value) {
                $test = $obj->setOption($option, $value);
                if (DB::isError($test)) {
                    return $test;
                }
            }
        } else {
            $obj->setOption('persistent', $options);
        }
        $err = $obj->connect($dsninfo, $obj->getOption('persistent'));

        if (DB::isError($err)) {
            $err->addUserInfo($dsn);
            return $err;
        }

        return $obj;
    }

    /**
     * Return the DB API version
     *
     * @return int the DB API version number
     */
    function apiVersion()
    {
        return 2;
    }

    /**
     * Tell whether a result code from a DB method is an error
     *
     * @param $value int result code
     *
     * @return bool whether $value is an error
     */
    function isError($value)
    {
        return (is_object($value) &&
                (get_class($value) == 'db_error' ||
                 is_subclass_of($value, 'db_error')));
    }

    /**
     * Tell whether a query is a data manipulation query (insert,
     * update or delete) or a data definition query (create, drop,
     * alter, grant, revoke).
     *
     * @access public
     *
     * @param string the query
     *
     * @return bool whether $query is a data manipulation query
     */
    function isManip($query)
    {
        if (preg_match('/^\s*"?(INSERT|UPDATE|DELETE|REPLACE|CREATE|DROP|ALTER|GRANT|REVOKE)\s+/i', $query)) {
            return true;
        }
        return false;
    }

    /**
     * Tell whether a result code from a DB method is a warning.
     * Warnings differ from errors in that they are generated by DB,
     * and are not fatal.
     *
     * @param $value mixed result value
     *
     * @return bool whether $value is a warning
     */
    function isWarning($value)
    {
        return is_object($value) &&
            (get_class( $value ) == "db_warning" ||
             is_subclass_of($value, "db_warning"));
    }

    /**
     * Return a textual error message for a DB error code
     *
     * @param $value int error code
     *
     * @return string error message, or false if the error code was
     * not recognized
     */
    function errorMessage($value)
    {
        if (!isset($errorMessages)) {
            $errorMessages = array(
                DB_ERROR                    => 'unknown error',
                DB_ERROR_ALREADY_EXISTS     => 'already exists',
                DB_ERROR_CANNOT_CREATE      => 'can not create',
                DB_ERROR_CANNOT_DELETE      => 'can not delete',
                DB_ERROR_CANNOT_DROP        => 'can not drop',
                DB_ERROR_CONSTRAINT         => 'constraint violation',
                DB_ERROR_DIVZERO            => 'division by zero',
                DB_ERROR_INVALID            => 'invalid',
                DB_ERROR_INVALID_DATE       => 'invalid date or time',
                DB_ERROR_INVALID_NUMBER     => 'invalid number',
                DB_ERROR_MISMATCH           => 'mismatch',
                DB_ERROR_NODBSELECTED       => 'no database selected',
                DB_ERROR_NOSUCHFIELD        => 'no such field',
                DB_ERROR_NOSUCHTABLE        => 'no such table',
                DB_ERROR_NOT_CAPABLE        => 'DB backend not capable',
                DB_ERROR_NOT_FOUND          => 'not found',
                DB_ERROR_NOT_LOCKED         => 'not locked',
                DB_ERROR_SYNTAX             => 'syntax error',
                DB_ERROR_UNSUPPORTED        => 'not supported',
                DB_ERROR_VALUE_COUNT_ON_ROW => 'value count on row',
                DB_ERROR_INVALID_DSN        => "invalid DSN",
                DB_ERROR_CONNECT_FAILED     => "connect failed",
                DB_OK                       => 'no error',
                DB_WARNING                  => 'unknown warning',
                DB_WARNING_READ_ONLY        => 'read only'
            );
        }

        if (DB::isError($value)) {
            $value = $value->getCode();
        }

        return $errorMessages[$value];
    }

    /**
     * Parse a data source name
     *
     * @param $dsn string Data Source Name to be parsed
     *
     * @return array an associative array with the following keys:
     *
     *  phptype: Database backend used in PHP (mysql, odbc etc.)
     *  dbsyntax: Database used with regards to SQL syntax etc.
     *  protocol: Communication protocol to use (tcp, unix etc.)
     *  hostspec: Host specification (hostname[:port])
     *  database: Database to use on the DBMS server
     *  username: User name for login
     *  password: Password for login
     *
     * The format of the supplied DSN is in its fullest form:
     *
     *  phptype(dbsyntax)://username:password@protocol+hostspec/database
     *
     * Most variations are allowed:
     *
     *  phptype://username:password@protocol+hostspec:110//usr/db_file.db
     *  phptype://username:password@hostspec/database_name
     *  phptype://username:password@hostspec
     *  phptype://username@hostspec
     *  phptype://hostspec/database
     *  phptype://hostspec
     *  phptype(dbsyntax)
     *  phptype
     *
     * @author Tomas V.V.Cox <cox@idecnet.com>
     */
    function parseDSN($dsn)
    {
        if (is_array($dsn)) {
            return $dsn;
        }

        $parsed = array(
            'phptype'  => false,
            'dbsyntax' => false,
            'protocol' => false,
            'hostspec' => false,
            'database' => false,
            'username' => false,
            'password' => false
        );

        // Find phptype and dbsyntax
        if (($pos = strpos($dsn, '://')) !== false) {
            $str = substr($dsn, 0, $pos);
            $dsn = substr($dsn, $pos + 3);
        } else {
            $str = $dsn;
            $dsn = NULL;
        }

        // Get phptype and dbsyntax
        // $str => phptype(dbsyntax)
        if (preg_match('|^(.+?)\((.*?)\)$|', $str, $arr)) {
            $parsed['phptype'] = $arr[1];
            $parsed['dbsyntax'] = (empty($arr[2])) ? $arr[1] : $arr[2];
        } else {
            $parsed['phptype'] = $str;
            $parsed['dbsyntax'] = $str;
        }

        if (empty($dsn)) {
            return $parsed;
        }

        // Get (if found): username and password
        // $dsn => username:password@protocol+hostspec/database
        if (($at = strpos($dsn,'@')) !== false) {
            $str = substr($dsn, 0, $at);
            $dsn = substr($dsn, $at + 1);
            if (($pos = strpos($str, ':')) !== false) {
                $parsed['username'] = urldecode(substr($str, 0, $pos));
                $parsed['password'] = urldecode(substr($str, $pos + 1));
            } else {
                $parsed['username'] = urldecode($str);
            }
        }

        // Find protocol and hostspec
        // $dsn => protocol+hostspec/database
        if (($pos=strpos($dsn, '/')) !== false) {
            $str = substr($dsn, 0, $pos);
            $dsn = substr($dsn, $pos + 1);
        } else {
            $str = $dsn;
            $dsn = NULL;
        }

        // Get protocol + hostspec
        // $str => protocol+hostspec
        if (($pos=strpos($str, '+')) !== false) {
            $parsed['protocol'] = substr($str, 0, $pos);
            $parsed['hostspec'] = urldecode(substr($str, $pos + 1));
        } else {
            $parsed['hostspec'] = urldecode($str);
        }

        // Get dabase if any
        // $dsn => database
        if (!empty($dsn)) {
            $parsed['database'] = $dsn;
        }

        return $parsed;
    }

    /**
     * Load a PHP database extension if it is not loaded already.
     *
     * @access public
     *
     * @param $name the base name of the extension (without the .so or
     * .dll suffix)
     *
     * @return bool true if the extension was already or successfully
     * loaded, false if it could not be loaded
     */
    function assertExtension($name)
    {
        if (!extension_loaded($name)) {
            $dlext = (substr(PHP_OS, 0, 3) == 'WIN') ? '.dll' : '.so';
            @dl($name . $dlext);
        }
        if (!extension_loaded($name)) {
            return false;
        }
        return true;
    }
}

/**
 * DB_Error implements a class for reporting portable database error
 * messages.
 *
 * @author Stig Bakken <ssb@fast.no>
 */
class DB_Error extends PEAR_Error
{
    /**
     * DB_Error constructor.
     *
     * @param $code mixed DB error code, or string with error message.
     * @param $mode int what "error mode" to operate in
     * @param $level what error level to use for $mode & PEAR_ERROR_TRIGGER
     * @param $debuginfo additional debug info, such as the last query
     *
     * @access public
     *
     * @see PEAR_Error
     */

    function DB_Error($code = DB_ERROR, $mode = PEAR_ERROR_RETURN,
              $level = E_USER_NOTICE, $debuginfo = null)
    {
        if (is_int($code)) {
            $this->PEAR_Error('DB Error: ' . DB::errorMessage($code), $code, $mode, $level, $debuginfo);
        } else {
            $this->PEAR_Error("DB Error: $code", DB_ERROR, $mode, $level, $debuginfo);
        }
    }
}

/**
 * DB_Warning implements a class for reporting portable database
 * warning messages.
 *
 * @author Stig Bakken <ssb@fast.no>
 */
class DB_Warning extends PEAR_Error
{
    /**
     * DB_Warning constructor.
     *
     * @param $code mixed DB error code, or string with error message.
     * @param $mode int what "error mode" to operate in
     * @param $level what error level to use for $mode == PEAR_ERROR_TRIGGER
     * @param $debuginfo additional debug info, such as the last query
     *
     * @access public
     *
     * @see PEAR_Error
     */

    function DB_Warning($code = DB_WARNING, $mode = PEAR_ERROR_RETURN,
            $level = E_USER_NOTICE, $debuginfo = null)
    {
        if (is_int($code)) {
            $this->PEAR_Error('DB Warning: ' . DB::errorMessage($code), $code, $mode, $level, $debuginfo);
        } else {
            $this->PEAR_Error("DB Warning: $code", 0, $mode, $level, $debuginfo);
        }
    }
}

/**
 * This class implements a wrapper for a DB result set.
 * A new instance of this class will be returned by the DB implementation
 * after processing a query that returns data.
 *
 * @author Stig Bakken <ssb@fast.no>
 */

class DB_result
{
    var $dbh;
    var $result;

    /**
     * DB_result constructor.
     * @param   $dbh    DB object reference
     * @param   $result result resource id
     */

    function DB_result(&$dbh, $result)
    {
        $this->dbh = &$dbh;
        $this->result = $result;
    }

    /**
     * Fetch and return a row of data (it uses fetchInto for that)
     * @param   $fetchmode  format of fetched row array
     * @param   $rownum     the absolute row number to fetch
     *
     * @return  array   a row of data, or false on error
     */
    function fetchRow($fetchmode = DB_FETCHMODE_DEFAULT, $rownum=null)
    {
        if ($fetchmode === DB_FETCHMODE_DEFAULT) {
            $fetchmode = $this->dbh->fetchmode;
        }

        $res = $this->dbh->fetchInto($this->result, $arr, $fetchmode, $rownum);
        if ($res !== DB_OK) {
            return $res;
        }
        return $arr;
    }

    /**
     * Fetch a row of data into an existing array.
     *
     * @param   $arr                reference to data array
     * @param   $fetchmode  format of fetched row array
     * @param   $rownum     the absolute row number to fetch
     *
     * @return  int     error code
     */
    function fetchInto(&$arr, $fetchmode = DB_FETCHMODE_DEFAULT, $rownum=null)
    {
        if ($fetchmode === DB_FETCHMODE_DEFAULT) {
            $fetchmode = $this->dbh->fetchmode;
        }
        return $this->dbh->fetchInto($this->result, $arr, $fetchmode, $rownum);
    }

    /**
     * Get the the number of columns in a result set.
     *
     * @return int the number of columns, or a DB error
     */
    function numCols()
    {
        return $this->dbh->numCols($this->result);
    }

    /**
     * Get the number of rows in a result set.
     *
     * @return int the number of rows, or a DB error
     */
    function numRows()
    {
        return $this->dbh->numRows($this->result);
    }

    /**
     * Frees the resources allocated for this result set.
     * @return  int     error code
     */
    function free()
    {
        $err = $this->dbh->freeResult($this->result);
        if(DB::isError($err)) {
            return $err;
        }
        $this->result = false;
        return true;
    }

    function tableInfo($mode = null)
    {
        return $this->dbh->tableInfo($this->result, $mode);
    }
}

?>
