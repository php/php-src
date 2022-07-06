--TEST--
Bug #48773 (Incorrect error when setting PDO::ATTR_STATEMENT_CLASS with ctor_args)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php

class bar extends PDOStatement {
    private function __construct() {
    }
}

class foo extends PDO {
    public $statementClass = 'bar';
    function __construct($dsn, $username, $password, $driver_options = array()) {
        $driver_options[PDO::ATTR_ERRMODE] = PDO::ERRMODE_EXCEPTION;
        parent::__construct($dsn, $username, $password, $driver_options);

        $this->setAttribute(PDO::ATTR_STATEMENT_CLASS, array($this->statementClass, array($this)));
    }
}

$db = new foo('sqlite::memory:', '', '');
$stmt = $db->query('SELECT 1');
var_dump($stmt);

?>
--EXPECTF--
object(bar)#%d (1) {
  ["queryString"]=>
  string(8) "SELECT 1"
}
