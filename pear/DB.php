<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// Database independent query interface.
//

// {{{ Database independent error codes.

define("DB_OK",                     0);
define("DB_ERROR",                 -1);
define("DB_ERROR_SYNTAX",          -2);
define("DB_ERROR_CONSTRAINT",      -3);
define("DB_ERROR_NOT_FOUND",       -4);
define("DB_ERROR_ALREADY_EXISTS",  -5);
define("DB_ERROR_UNSUPPORTED",     -6);
define("DB_ERROR_MISMATCH",        -7);
define("DB_ERROR_INVALID",         -8);
define("DB_ERROR_NOT_CAPABLE",     -9);
define("DB_ERROR_TRUNCATED",      -10);
define("DB_ERROR_INVALID_NUMBER", -11);
define("DB_ERROR_INVALID_DATE",   -12);
define("DB_ERROR_DIVZERO",        -13);
define("DB_ERROR_NODBSELECTED",   -14);

// }}}
// {{{ Prepare/execute parameter types

define("DB_PARAM_SCALAR",           1);
define("DB_PARAM_OPAQUE",           2);

// }}}
// {{{ Binary data modes

define("DB_BINMODE_PASSTHRU",       1);
define("DB_BINMODE_RETURN",         2);
define("DB_BINMODE_CONVERT",        3);

// }}}

// {{{ class DB

/**
 * This class implements a factory method for creating DB objects,
 * as well as some "static methods".
 *
 * @version  100
 * @author   Stig Bakken <ssb@fast.no>
 * @since    4.0b4
 */
class DB {
	/**
	 * Create a new DB object for the specified database type.
	 * @param   $type   database type
	 * @return  object  a newly created DB object, or false on error
	 */
    function factory($type) {
		global $USED_PACKAGES;
		// "include" should be replaced with "use" once PHP gets it
		$pkgname = 'DB/' . $type;
		if (!is_array($USED_PACKAGES) || !$USED_PACKAGES[$pkgname]) {
			if (!@include("DB/${type}.php")) {
				return DB_ERROR_NOT_FOUND;
			} else {
				$USED_PACKAGES[$pkgname] = true;
			}
		}
		$classname = 'DB_' . $type;
		$obj = new $classname;
		return $obj;
    }

	function connect($dsn, $persistent = false) {
		global $USED_PACKAGES;

		$dsninfo = DB::parseDSN($dsn);
		$type = $dsninfo['phptype'];
		// "include" should be replaced with "use" once PHP gets it
		$pkgname = 'DB/' . $type;
		if (!is_array($USED_PACKAGES) || !$USED_PACKAGES[$pkgname]) {
			print "${pkgname}.php\n";
			if (!@include($pkgname . '.php')) {
				return DB_ERROR_NOT_FOUND;
			} else {
				$USED_PACKAGES[$pkgname] = true;
			}
		}
		$classname = 'DB_' . $type;
		$obj = new $classname;
		$obj->connect(&$dsninfo, $persistent);
		return $obj; // XXX ADDREF
	}

	/**
	 * Return the DB API version.
	 * @return  int     the DB API version number
	 */
    function apiVersion() {
		return 100;
    }

	/**
	 * Tell whether a result code from a DB method is an error.
	 * @param   $code   result code
	 * @return  bool    whether $code is an error
	 */
	function isError($code) {
		return is_int($code) && ($code < 0);
	}

	/**
	 * Return a textual error message for an error code.
	 * @param   $code   error code
	 * @return  string  error message
	 */
	function errorMessage($code) {
		if (!is_array($errorMessages)) {
			$errorMessages = array(
				DB_OK                   => "no error",
				DB_ERROR                => "unknown error",
				DB_ERROR_SYNTAX         => "syntax error",
				DB_ERROR_CONSTRAINT     => "constraint violation",
				DB_ERROR_NOT_FOUND      => "not found",
				DB_ERROR_ALREADY_EXISTS => "already exists",
				DB_ERROR_UNSUPPORTED    => "not supported",
				DB_ERROR_MISMATCH       => "mismatch",
				DB_ERROR_INVALID        => "invalid",
				DB_ERROR_NOT_CAPABLE    => "DB implementation not capable",
				DB_ERROR_INVALID_NUMBER => "invalid number",
				DB_ERROR_INVALID_DATE   => "invalid date or time",
				DB_ERROR_DIVZERO        => "division by zero",
				DB_ERROR_NODBSELECTED   => "no database selected"
			);
		}
		return $errorMessages[$code];
	}

	/**
	 * Parse a data source name and return an associative array with
	 * the following keys:
	 *  phptype       Database backend used in PHP (mysql, odbc etc.)
	 *  dbsyntax      Database used with regards to SQL syntax etc.
	 *  protocol      Communication protocol to use (tcp, unix etc.)
	 *  hostspec      Host specification (hostname[:port])
	 *  database      Database to use on the DBMS server
	 *  username      User name for login
     *  password      Password for login
	 *
	 * The format of the supplied DSN is in its fullest form:
	 *  phptype(dbsyntax)://username:password@protocol+hostspec/database
	 * Most variations are allowed:
	 *  phptype://username:password@protocol+hostspec/database
	 *  phptype://username:password@hostspec/database
	 *  phptype://username:password@hostspec
	 *  phptype://hostspec/database
	 *  phptype://hostspec
	 *  phptype(dbsyntax)
	 *  phptype
	 *
	 * FALSE is returned on error.
	 *
	 * @param $dsn Data Source Name to be parsed
	 */
	function parseDSN($dsn) {
		$parsed = array(
			'phptype'  => false,
			'dbsyntax' => false,
			'protocol' => false,
			'hostspec' => false,
			'database' => false,
			'username' => false,
			'password' => false
		);
		if (preg_match('|^([^:]+)://|', $dsn, &$arr)) {
			$dbtype = $arr[1];
			$dsn = preg_replace('|^[^:]+://|', '', $dsn);
			// match "phptype(dbsyntax)"
			if (preg_match('|^([^\(]+)\((.+)\)$|', $dbtype, &$arr)) {
				$parsed['phptype'] = $arr[1];
				$parsed['dbsyntax'] = $arr[2];
			} else {
				$parsed['phptype'] = $dbtype;
			}
		} else {
			// match "phptype(dbsyntax)"
			if (preg_match('|^([^\(]+)\((.+)\)$|', $dsn, &$arr)) {
				$parsed['phptype'] = $arr[1];
				$parsed['dbsyntax'] = $arr[2];
			} else {
				$parsed['phptype'] = $dsn;
			}
			return $parsed; // XXX ADDREF
		}

		if (preg_match('|^(.*)/([^/]+)$|', $dsn, &$arr)) {
			$parsed['database'] = $arr[2];
			$dsn = $arr[1];
		}

		if (preg_match('|^([^:]+):([^@]+)@?(.*)$|', $dsn, &$arr)) {
			$parsed['username'] = $arr[1];
			$parsed['password'] = $arr[2];
			$dsn = $arr[3];
		} elseif (preg_match('|^([^:]+)@(.*)$|', $dsn, &$arr)) {
			$parsed['username'] = $arr[1];
			$dsn = $arr[3];
		}

		if (preg_match('|^([^\+]+)\+(.*)$|', $dsn, &$arr)) {
			$parsed['protocol'] = $arr[1];
			$dsn = $arr[2];
		}

		$parsed['hostspec'] = $dsn;

		if (!$parsed['dbsyntax']) {
			$parsed['dbsyntax'] = $parsed['phptype'];
		}

		return $parsed; // XXX ADDREF
	}
}

// }}}
// {{{ class DB_result

/**
 * This class implements a wrapper for a DB result set.
 * A new instance of this class will be returned by the DB implementation
 * after processing a query that returns data.
 */
class DB_result {
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
    function fetchRow() {
		return $this->dbh->fetchRow($this->result);
    }

    /**
	 * Fetch a row of data into an existing array.
	 * @param   $arr    reference to data array
	 * @return  int     error code
	 */
    function fetchInto(&$arr) {
		return $this->dbh->fetchInto($this->result, &$arr);
    }

    /**
	 * Frees the resource for this result and reset ourselves.
	 * @return  int     error code
	 */
    function free() {
		$err = $this->dbh->freeResult($this->result);
		if (DB::isError($err)) {
			return $err;
		}
		$this->dbh = $this->result = false;
		return true;
    }
}

// }}}

// Local variables:
// tab-width: 4
// c-basic-offset: 4
// End:
?>
