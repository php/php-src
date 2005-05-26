<?php

class PDOStatementX extends PDOStatement
{
    public $test1 = 1;
    
    protected function __construct()
    {
    	$this->test2 = 2;
    	$this->test2 = 22;
    	echo __METHOD__ . "()\n";
    }
    
    function __destruct()
    {
    	echo __METHOD__ . "()\n";
    }
}

class PDODatabaseX extends PDO
{
    public $test1 = 1;
    
    function __destruct()
    {
    	echo __METHOD__ . "()\n";
    }
    
    function test()
    {
    	$this->test2 = 2;
        var_dump($this->test1);
        var_dump($this->test2);
    	$this->test2 = 22;
    }
    
    function query($sql)
    {
    	echo __METHOD__ . "()\n";
    	$stmt = parent::prepare($sql, array(PDO_ATTR_STATEMENT_CLASS=>array('PDOStatementx')));
    	$stmt->execute();
    	return $stmt;
    }
}

?>
