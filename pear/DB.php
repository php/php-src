<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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
// |                                                                      |
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

define("DB_OK",                         0);
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

/*
 * Warnings are not detected as errors by DB::isError(), and are not
 * fatal.  You can detect whether an error is in fact a warning with
 * DB::isWarning().
 */

define("DB_WARNING",           -1000);
define("DB_WARNING_READ_ONLY", -1001);

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

define("DB_PARAM_SCALAR", 1);
define("DB_PARAM_OPAQUE", 2);

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

define("DB_BINMODE_PASSTHRU", 1);
define("DB_BINMODE_RETURN",   2);
define("DB_BINMODE_CONVERT",  3);

/**
 * This is a special constant that tells DB the user hasn't specified
 * any particular get mode, so the default should be used.
 */

define("DB_FETCHMODE_DEFAULT", 0);

/**
 * Column data indexed by numbers, ordered from 0 and up
 */

define("DB_FETCHMODE_ORDERED", 1);

/**
 * Column data indexed by column names
 */

define("DB_FETCHMODE_ASSOC", 2);

/**
 * For multi-dimensional results: normally the first level of arrays
 * is the row number, and the second level indexed by column number or name.
 * DB_FETCHMODE_FLIPPED switches this order, so the first level of arrays
 * is the column name, and the second level the row number.
 */

define("DB_FETCHMODE_FLIPPED", 4);

/* for compatibility */

define("DB_GETMODE_ORDERED", DB_FETCHMODE_ORDERED);
define("DB_GETMODE_ASSOC",   DB_FETCHMODE_ASSOC);
define("DB_GETMODE_FLIPPED", DB_FETCHMODE_FLIPPED);

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

	$classname = "DB_" . $type;
	$obj = @new $classname;

	if (!$obj) {
	    return new DB_Error(DB_ERROR_NOT_FOUND);
	}

	return $obj;
    }

    /**
     * Create a new DB object and connect to the specified database
     *
     * @param $dsn mixed "data source name", see the DB::parseDSN
     * method for a description of the dsn format.  Can also be
     * specified as an array of the format returned by DB::parseDSN.
     *
     * @param $persistent bool whether this connection should be
     * persistent.  Ignored if the backend extension does not support
     * persistent connections.
     *
     * @return object a newly created DB object, or a DB error code on
     * error
     *
     * @see DB::parseDSN
     */
    function &connect($dsn, $persistent = false)
    {
	if (is_array($dsn)) {
	    $dsninfo = $dsn;
	} else {
	    $dsninfo = DB::parseDSN($dsn);
	}
	$type = $dsninfo["phptype"];
	$classname = "DB_" . $type;

	@include_once("DB/${type}.php");
	$obj = @new $classname;

	if (!$obj) {
	    return new DB_Error(DB_ERROR_NOT_FOUND);
	}

	$err = $obj->connect($dsninfo, $persistent);

	if (DB::isError($err)) {
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
	return is_object($value) &&
	    (get_class($value) == "db_error" ||
	     is_subclass_of($value, "db_error"));
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
		DB_ERROR                    => "unknown error",
		DB_ERROR_ALREADY_EXISTS     => "already exists",
		DB_ERROR_CANNOT_CREATE      => "can not create",
		DB_ERROR_CANNOT_DELETE      => "can not delete",
		DB_ERROR_CANNOT_DROP        => "can not drop",
		DB_ERROR_CONSTRAINT         => "constraint violation",
		DB_ERROR_DIVZERO            => "division by zero",
		DB_ERROR_INVALID            => "invalid",
		DB_ERROR_INVALID_DATE       => "invalid date or time",
		DB_ERROR_INVALID_NUMBER     => "invalid number",
		DB_ERROR_MISMATCH           => "mismatch",
		DB_ERROR_NODBSELECTED       => "no database selected",
		DB_ERROR_NOSUCHFIELD        => "no such field",
		DB_ERROR_NOSUCHTABLE        => "no such table",
		DB_ERROR_NOT_CAPABLE        => "DB backend not capable",
		DB_ERROR_NOT_FOUND          => "not found",
		DB_ERROR_NOT_LOCKED         => "not locked",
		DB_ERROR_SYNTAX             => "syntax error",
		DB_ERROR_UNSUPPORTED        => "not supported",
		DB_ERROR_VALUE_COUNT_ON_ROW => "value count on row",
		DB_OK                       => "no error",
		DB_WARNING                  => "unknown warning",
		DB_WARNING_READ_ONLY        => "read only"
	    );
	}

	if (DB::isError($value)) {
	    $value = $value->code;
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
     * phptype: Database backend used in PHP (mysql, odbc etc.)
     * dbsyntax: Database used with regards to SQL syntax etc.
     * protocol: Communication protocol to use (tcp, unix etc.)
     * hostspec: Host specification (hostname[:port])
     * database: Database to use on the DBMS server
     * username: User name for login
     * password: Password for login
     *
     * The format of the supplied DSN is in its fullest form:
     *
     *  phptype(dbsyntax)://username:password@protocol+hostspec/database
     *
     * Most variations are allowed:
     *  phptype://username:password@protocol+hostspec/database</li>
     *  phptype://username:password@hostspec/database</li>
     *  phptype://username:password@hostspec</li>
     *  phptype://hostspec/database</li>
     *  phptype://hostspec</li>
     *  phptype(dbsyntax)</li>
     *  phptype</li>
     *
     * @return bool FALSE is returned on error
     */
    function parseDSN($dsn)
    {
	if (is_array($dsn)) {
	    return $dsn;
	}
    
	$parsed = array(
	    "phptype"  => false,
	    "dbsyntax" => false,
	    "protocol" => false,
	    "hostspec" => false,
	    "database" => false,
	    "username" => false,
	    "password" => false
	);

	if (preg_match("|^([^:]+)://|", $dsn, $arr)) {
	    $dbtype = $arr[ 1 ];
	    $dsn = preg_replace( "|^[^:]+://|", '', $dsn);
	    
	    // match "phptype(dbsyntax)"
	    if (preg_match("|^([^\(]+)\((.+)\)$|", $dbtype, $arr)) {
		$parsed["phptype"] = $arr[1];
		$parsed["dbsyntax"] = $arr[2];
	    } else {
		$parsed["phptype"] = $dbtype;
	    }
        } else {
	    // match "phptype(dbsyntax)"
	    if (preg_match("|^([^\(]+)\((.+)\)$|", $dsn, $arr)) {
		$parsed["phptype"] = $arr[1];
		$parsed["dbsyntax"] = $arr[2];
	    } else {
		$parsed["phptype"] = $dsn;
	    }

	    return $parsed;
        }

        if (preg_match("|^(.*)/([^/]+)/?$|", $dsn, $arr)) {
	    $parsed["database"] = $arr[2];
	    $dsn = $arr[1];
        }

        if (preg_match("|^([^:]+):([^@]+)@?(.*)$|", $dsn, $arr)) {
	    $parsed["username"] = $arr[1];
	    $parsed["password"] = $arr[2];
	    $dsn = $arr[3];
        } elseif (preg_match("|^([^:]+)@(.*)$|", $dsn, $arr)) {
	    $parsed["username"] = $arr[1];
	    $dsn = $arr[2];
        }

        if (preg_match("|^([^\+]+)\+(.*)$|", $dsn, $arr)) {
	    $parsed["protocol"] = $arr[1];
	    $dsn = $arr[2];
        }

        if (!$parsed["database"]) {
	    $dsn = preg_replace("|/+$|", "", $dsn);
        }

        $parsed["hostspec"] = urldecode($dsn);

        if(!$parsed["dbsyntax"]) {
	    $parsed["dbsyntax"] = $parsed["phptype"];
        }

        return $parsed;
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
		      $level = E_USER_NOTICE, $debuginfo = null) {
	if (is_int($code)) {
	    $this->PEAR_Error("DB Error: " . DB::errorMessage( $code ), $code, $mode, $level, $debuginfo);
	} else {
	    $this->PEAR_Error("DB Error: $code", 0, $mode, $level, $debuginfo);
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
			$level = E_USER_NOTICE, $debuginfo = null) {
      if (is_int($code)) {
	  $this->PEAR_Error("DB Warning: " . DB::errorMessage( $code ), $code, $mode, $level, $debuginfo);
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

    function DB_result(&$dbh, $result) {
	$this->dbh = &$dbh;
	$this->result = $result;
    }

    /**
     * Fetch and return a row of data.
     * @return  array   a row of data, or false on error
     */
    function fetchRow($fetchmode = DB_FETCHMODE_DEFAULT) {
	if ($fetchmode == DB_FETCHMODE_DEFAULT) {
	    $fetchmode = $this->dbh->fetchmode;
	}
	return $this->dbh->fetchRow($this->result, $fetchmode);
    }

    /**
     * Fetch a row of data into an existing array.
     *
     * @param   $arr    reference to data array
     * @return  int     error code
     */
    function fetchInto(&$arr, $fetchmode = DB_FETCHMODE_DEFAULT) {
	if ($fetchmode == DB_FETCHMODE_DEFAULT) {
	    $fetchmode = $this->dbh->fetchmode;
	}
	return $this->dbh->fetchInto($this->result, $arr, $fetchmode);
    }

    /**
     * Get the the number of columns in a result set.
     *
     * @return int the number of columns, or a DB error code
     */
    function numCols() {
	return $this->dbh->numCols($this->result);
    }

    /**
     * Frees the resources allocated for this result set.
     * @return  int     error code
     */
    function free() {
	$err = $this->dbh->freeResult($this->result);
	if(DB::isError($err)) {
	    return $err;
	}
	$this->result = false;
	return true;
    }
}

?>
