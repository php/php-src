<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
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
define("DB_ERROR_EXTENSION_NOT_FOUND",-25);

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
 * There are two extensions though, a "&" character means an opaque
 * parameter.  An opaque parameter is simply a file name, the real
 * data are in that file (useful for putting uploaded files into your
 * database and such). The "!" char means a parameter that must be
 * left as it is.
 * They modify the quote behavoir:
 * DB_PARAM_SCALAR (?) => 'original string quoted'
 * DB_PARAM_OPAQUE (&) => 'string from file quoted'
 * DB_PARAM_MISC   (!) => original string
 */

define('DB_PARAM_SCALAR', 1);
define('DB_PARAM_OPAQUE', 2);
define('DB_PARAM_MISC',   3);

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
* Column data as object properties
*/

define('DB_FETCHMODE_OBJECT', 3);

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
 * @package  DB
 * @version  2
 * @author   Stig Bakken <ssb@fast.no>
 * @since    PHP 4.0
 */

class DB
{
    /**
     * Create a new DB connection object for the specified database
     * type
     *
     * @param string $type database type, for example "mysql"
     *
     * @return mixed a newly created DB object, or a DB error code on
     * error
     *
     * access public
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
     * Create a new DB connection object and connect to the specified
     * database
     *
     * @param mixed $dsn "data source name", see the DB::parseDSN
     * method for a description of the dsn format.  Can also be
     * specified as an array of the format returned by DB::parseDSN.
     *
     * @param mixed $options An associative array of option names and
     * their values.  For backwards compatibility, this parameter may
     * also be a boolean that tells whether the connection should be
     * persistent.  See DB_common::setOption for more information on
     * connection options.
     *
     * @return mixed a newly created DB connection object, or a DB
     * error object on error
     *
     * @see DB::parseDSN
     * @see DB::isError
     * @see DB_common::setOption
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
     *
     * @access public
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
     *
     * @access public
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
     * @param string $query the query
     *
     * @return boolean whether $query is a data manipulation query
     */
    function isManip($query)
    {
        $manips = 'INSERT|UPDATE|DELETE|'.'REPLACE|CREATE|DROP|'.
                  'ALTER|GRANT|REVOKE|'.'LOCK|UNLOCK';
        if (preg_match('/^\s*"?('.$manips.')\s+/i', $query)) {
            return true;
        }
        return false;
    }

    /**
     * Tell whether a result code from a DB method is a warning.
     * Warnings differ from errors in that they are generated by DB,
     * and are not fatal.
     *
     * @param mixed $value result value
     *
     * @return boolean whether $value is a warning
     *
     * @access public
     */
    function isWarning($value)
    {
        return (is_object($value) &&
                (get_class($value) == "db_warning" ||
                 is_subclass_of($value, "db_warning")));
    }

    /**
     * Return a textual error message for a DB error code
     *
     * @param integer $value error code
     *
     * @return string error message, or false if the error code was
     * not recognized
     */
    function errorMessage($value)
    {
        static $errorMessages;
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
                DB_ERROR_INVALID_DSN        => 'invalid DSN',
                DB_ERROR_CONNECT_FAILED     => 'connect failed',
                DB_OK                       => 'no error',
                DB_WARNING                  => 'unknown warning',
                DB_WARNING_READ_ONLY        => 'read only',
                DB_ERROR_NEED_MORE_DATA     => 'insufficient data supplied',
                DB_ERROR_EXTENSION_NOT_FOUND=> 'extension not found'
            );
        }

        if (DB::isError($value)) {
            $value = $value->getCode();
        }

        return isset($errorMessages[$value]) ? $errorMessages[$value] : $errorMessages[DB_ERROR];
    }

    /**
     * Parse a data source name
     *
     * A array with the following keys will be returned:
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
     * @param string $dsn Data Source Name to be parsed
     *
     * @return array an associative array
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
            'username' => false,
            'password' => false,
            'protocol' => false,
            'hostspec' => false,
            'port'     => false,
            'socket'   => false,
            'database' => false
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
            $parsed['phptype']  = $arr[1];
            $parsed['dbsyntax'] = (empty($arr[2])) ? $arr[1] : $arr[2];
        } else {
            $parsed['phptype']  = $str;
            $parsed['dbsyntax'] = $str;
        }

        if (empty($dsn)) {
            return $parsed;
        }

        // Get (if found): username and password
        // $dsn => username:password@protocol+hostspec/database
        if (($at = strrpos($dsn,'@')) !== false) {
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

        // $dsn => proto(proto_opts)/database
        if (preg_match('|^(.+?)\((.*?)\)/?(.*?)$|', $dsn, $match)) {
            $proto       = $match[1];
            $proto_opts  = (!empty($match[2])) ? $match[2] : false;
            $dsn         = $match[3];

        // $dsn => protocol+hostspec/database (old format)
        } else {
            if (strpos($dsn, '+') !== false) {
                list($proto, $dsn) = explode('+', $dsn, 2);
            }
            if (strpos($dsn, '/') !== false) {
                list($proto_opts, $dsn) = explode('/', $dsn, 2);
            } else {
                $proto_opts = $dsn;
                $dsn = null;
            }
        }

        // process the different protocol options
        $parsed['protocol'] = (!empty($proto)) ? $proto : 'tcp';
        $proto_opts = urldecode($proto_opts);
        if ($parsed['protocol'] == 'tcp') {
            if (strpos($proto_opts, ':') !== false) {
                list($parsed['hostspec'], $parsed['port']) = explode(':', $proto_opts);
            } else {
                $parsed['hostspec'] = $proto_opts;
            }
        } elseif ($parsed['protocol'] == 'unix') {
            $parsed['socket'] = $proto_opts;
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
     * @param string $name the base name of the extension (without the .so or
     *                     .dll suffix)
     *
     * @return boolean true if the extension was already or successfully
     *                 loaded, false if it could not be loaded
     */
    function assertExtension($name)
    {
        if (!extension_loaded($name)) {
            $dlext = OS_WINDOWS ? '.dll' : '.so';
            @dl($name . $dlext);
        }
        return extension_loaded($name);
    }
}

/**
 * DB_Error implements a class for reporting portable database error
 * messages.
 *
 * @package  DB
 * @author Stig Bakken <ssb@fast.no>
 */
class DB_Error extends PEAR_Error
{
    /**
     * DB_Error constructor.
     *
     * @param mixed   $code      DB error code, or string with error message.
     * @param integer $mode      what "error mode" to operate in
     * @param integer $level     what error level to use for $mode & PEAR_ERROR_TRIGGER
     * @param smixed  $debuginfo additional debug info, such as the last query
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
 * @package  DB
 * @author Stig Bakken <ssb@fast.no>
 */
class DB_Warning extends PEAR_Error
{
    /**
     * DB_Warning constructor.
     *
     * @param mixed   $code      DB error code, or string with error message.
     * @param integer $mode      what "error mode" to operate in
     * @param integer $level     what error level to use for $mode == PEAR_ERROR_TRIGGER
     * @param mmixed  $debuginfo additional debug info, such as the last query
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
 * @package  DB
 * @author Stig Bakken <ssb@fast.no>
 */

class DB_result
{
    var $dbh;
    var $result;
    var $row_counter = null;

    /**
     * DB_result constructor.
     * @param   resource $dbh  DB object reference
     * @param   resource $result result resource id
     */

    function DB_result(&$dbh, $result)
    {
        $this->dbh = &$dbh;
        $this->result = $result;
    }

    /**
     * Fetch and return a row of data (it uses driver->fetchInto for that)
     * @param int $fetchmode  format of fetched row
     * @param int $rownum     the row number to fetch
     *
     * @return  array a row of data, NULL on no more rows or PEAR_Error on error
     *
     * @access public
     */
    function fetchRow($fetchmode = DB_FETCHMODE_DEFAULT, $rownum=null)
    {
        if ($fetchmode === DB_FETCHMODE_DEFAULT) {
            $fetchmode = $this->dbh->fetchmode;
        }
        if ($fetchmode === DB_FETCHMODE_OBJECT) {
            $fetchmode = DB_FETCHMODE_ASSOC;
            $object_class = $this->dbh->fetchmode_object_class;
        }
        if ($this->dbh->limit_from !== null) {
            if ($this->row_counter === null) {
                $this->row_counter = $this->dbh->limit_from;
                // For Interbase
                if ($this->dbh->features['limit'] == false) {
                    $i = 0;
                    while ($i++ < $this->dbh->limit_from) {
                        $this->dbh->fetchInto($this->result, $arr, $fetchmode);
                    }
                }
            }
            if ($this->row_counter >= (
                    $this->dbh->limit_from + $this->dbh->limit_count))
            {
                return null;
            }
            if ($this->dbh->features['limit'] == 'emulate') {
                $rownum = $this->row_counter;
            }

            $this->row_counter++;
        }
        $res = $this->dbh->fetchInto($this->result, $arr, $fetchmode, $rownum);
        if ($res !== DB_OK) {
            return $res;
        }
        if (isset($object_class)) {
            // default mode specified in DB_common::fetchmode_object_class property
            if ($object_class == 'stdClass') {
                $ret = (object) $arr;
            } else {
                $ret =& new $object_class($arr);
            }
            return $ret;
        }
        return $arr;
    }

    /**
     * Fetch a row of data into an existing variable.
     *
     * @param  mixed     $arr        reference to data containing the row
     * @param  integer   $fetchmode  format of fetched row
     * @param  integer   $rownum     the row number to fetch
     *
     * @return  mixed  DB_OK on success, NULL on no more rows or
     *                 a DB_Error object on error
     *
     * @access public
     */
    function fetchInto(&$arr, $fetchmode = DB_FETCHMODE_DEFAULT, $rownum=null)
    {
        if ($fetchmode === DB_FETCHMODE_DEFAULT) {
            $fetchmode = $this->dbh->fetchmode;
        }
        if ($fetchmode === DB_FETCHMODE_OBJECT) {
            $fetchmode = DB_FETCHMODE_ASSOC;
            $object_class = $this->dbh->fetchmode_object_class;
        }
        if ($this->dbh->limit_from !== null) {
            if ($this->row_counter === null) {
                $this->row_counter = $this->dbh->limit_from;
                // For Interbase
                if ($this->dbh->features['limit'] == false) {
                    $i = 0;
                    while ($i++ < $this->dbh->limit_from) {
                        $this->dbh->fetchInto($this->result, $arr, $fetchmode);
                    }
                }
            }
            if ($this->row_counter >= (
                    $this->dbh->limit_from + $this->dbh->limit_count))
            {
                return null;
            }
            if ($this->dbh->features['limit'] == 'emulate') {
                $rownum = $this->row_counter;
            }

            $this->row_counter++;
        }
        $res = $this->dbh->fetchInto($this->result, $arr, $fetchmode, $rownum);
        if (($res === DB_OK) && isset($object_class)) {
            // default mode specified in DB_common::fetchmode_object_class property
            if ($object_class == 'stdClass') {
                $arr = (object) $arr;
            } else {
                $arr = new $object_class($arr);
            }
        }
        return $res;
    }

    /**
     * Get the the number of columns in a result set.
     *
     * @return int the number of columns, or a DB error
     *
     * @access public
     */
    function numCols()
    {
        return $this->dbh->numCols($this->result);
    }

    /**
     * Get the number of rows in a result set.
     *
     * @return int the number of rows, or a DB error
     *
     * @access public
     */
    function numRows()
    {
        return $this->dbh->numRows($this->result);
    }

    /**
     * Get the next result if a batch of queries was executed.
     *
     * @return bool true if a new result is available or false if not.
     *
     * @access public
     */
    function nextResult()
    {
        return $this->dbh->nextResult($this->result);
    }

    /**
     * Frees the resources allocated for this result set.
     * @return  int     error code
     *
     * @access public
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

    /**
    * @deprecated
    */
    function tableInfo($mode = null)
    {
        return $this->dbh->tableInfo($this->result, $mode);
    }

    /**
    * returns the actual rows number
    * @return integer
    */
    function getRowCounter()
    {
        return $this->row_counter;
    }
}

/**
* Pear DB Row Object
* @see DB_common::setFetchMode()
*/
class DB_row
{
    /**
    * constructor
    *
    * @param resource row data as array
    */
    function DB_row(&$arr)
    {
        for (reset($arr); $key = key($arr); next($arr)) {
            $this->$key = &$arr[$key];
        }
    }
}

?>
