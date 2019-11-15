<?php

class PDO
{
	public function __construct(string $dsn, ?string $username = null, ?string $passwd = null, ?array $options = null);

	/**
	 * @param string $statement
	 * @param array $options
	 *
	 * @return PDOStatement|false
	 */
	public function prepare(string $statement, array $options = []);

	/**
	 * @return bool
	 */
	public function beginTransaction();

	/**
	 * @return bool
	 */
	public function commit();

	/**
	 * @return bool
	 */
	public function rollBack();

	/**
	 * @return bool
	 */
	public function inTransaction();

	/**
	 * @param int $attribute
	 * @param mixed $value
	 *
	 * @return bool
	 */
	public function setAttribute(int $attribute, $value);

	/**
	 * @param string $query
	 *
	 * @return int|false
	 */
	public function exec(string $query);

	/**
	 * @param string $sql
	 *
	 * @return object
	 */
	public function query(string $sql);

	/**
	 * @param ?string $seqname = null
	 *
	 * @return int|false
	 */
	public function lastInsertId(?string $seqname = null);

	/**
	 * @return string|null
	 */
	public function errorCode();

	/**
	 * @return array
	 */
	public function errorInfo();

	/**
	 * @param int $attribute
	 *
	 * @return mixed
	 */
	public function getAttribute(int $attribute);

	/**
	 * @param string $string
	 * @param int $paramtype
	 *
	 * @return string|false
	 */
	public function quote(string $string, int $paramtype = 0);

	/**
	 * @return array
	 */
	public static function getAvailableDrivers();
}

class PDOException
{
}

class PDOStatement
{
	/**
	 * @param array $bound_input_params
	 *
	 * @return bool
	 */
	public function execute(?array $bound_input_params = null);

	/**
	 * @param ?int $how = PDO_FETCH_BOTH
	 * @param ?int $orientation
	 * @param ?int $offset
	 *
	 * @return mixed
	 */
	public function fetch(int $how = PDO_FETCH_BOTH, int $orientation = 0, int $offset = 0);

    /**
	 * @param int|string $paramno
	 * @param mixed &$param
	 * @param int $type
	 * @param int $maxlen
	 * @param ?mixed $driverdata
	 *
	 * @return bool
	 */
	public function bindParam($paramno, &$param, $type = 0, $maxlen = 0, $driverdata = null);

	/**
	 * @param int|string $paramno
	 * @param mixed &$param
	 * @param int $type
	 * @param int $maxlen
	 * @param ?mixed $driverdata
	 *
	 * @return bool
	 */
	public function bindColumn($paramno, &$param, $type = 0, $maxlen = 0, $driverdata = null);

	/**
	 * @param int|string $paramno
	 * @param mixed $param
	 * @param int $type
	 *
	 * @return bool
	 */
	public function bindValue($paramno, $param, int $type = 0);

    /**
     * @return int
     */
	public function rowCount();

	/**
	 * @param int $column_number
	 *
	 * @return string|false
	 */
	public function fetchColumn(int $column_number = 0);

	/**
	 * @param int $how
	 * @param string $class_name
	 * @param ?array $ctor_args
	 *
	 * @return array|false
	 */
	public function fetchAll($how = PDO_FETCH_BOTH, $class_name = '', $ctor_args = null);

	/**
	 * @param string $class_name
	 * @param ?array $ctor_args
	 *
	 * @return mixed
	 */
	public function fetchObject(string $class_name = '', ?array $ctor_args = null);

	/**
	 * @return string|null
	 */
	public function errorCode();

	/**
	 * @return array
	 */
	public function errorInfo();

	/**
	 * @param int $attribute
	 * @param mixed $value
	 *
	 * @return bool
	 */
	public function setAttribute(int $attribute, mixed $value);

	/**
	 * @param int $attribute
	 *
	 * @return mixed
	 */
	public function getAttribute(int $attribute);

	/**
	 * @return int
	 */
	public function columnCount();

	/**
	 * @param int $column
	 *
	 * @return array|false
	 */
	public function getColumnMeta(int $column);

	/**
	 * @param int $mode
	 * @param ?mixed $params
	 *
	 * @return bool
	 */
	public function setFetchMode($mode, $params = null);

	/**
	 * @return bool
	 */
	public function nextRowset();

	/**
	 * @return bool
	 */
	public function closeCursor();

	/**
	 * @return void|false
	 */
	public function debugDumpParams();
}

class PDORow
{
}
