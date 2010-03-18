--TEST--
Bug #49521 (PDO fetchObject sets values before calling constructor)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) die ("skip Need PDO_SQlite support");
?>
--FILE--
<?php

class Book {
    public $title = 'test';
    public $author;
    
	public function __construct($x) {
		$this->title = '';
		echo __METHOD__,"\n";
	}
	public function __set($a, $b) {
		echo __METHOD__,"\n";
		var_dump($a);
	}
}

$pdo = new PDO('sqlite::memory:');
$pdo->exec('CREATE TABLE book(title,author)');
$pdo->exec('INSERT INTO book VALUES ("PHP","Rasmus")');
$statement = $pdo->prepare('SELECT * FROM book WHERE title="PHP"');
$statement->execute();
var_dump($statement->fetchObject('Book', array(1)));

?>
--EXPECTF--
Book::__construct
object(Book)#%d (2) {
  [%u|b%"title"]=>
  string(0) ""
  [%u|b%"author"]=>
  string(6) "Rasmus"
}
