--TEST--
GH-7803: PDO __call breaks method calls
--FILE--
<?php

class DB extends PDO
{
    public function __call(string $name, array $args)
    {
        echo "__call: $name\n";
    }
}

$db = new DB('sqlite::memory:', '', '', []);

$db->sqliteCreateFunction('CONCAT', function (...$args) {
    return implode('', $args);
});

$sqlite = 'sqlite';
$createFunction = 'CreateFunction';
$db->{$sqlite . $createFunction}('CONCAT2', function () {});

$db->nonexistent();

var_dump($db->query('SELECT CONCAT(\'123\', \'456\', \'789\')')->fetch());

?>
--EXPECT--
__call: nonexistent
array(2) {
  ["CONCAT('123', '456', '789')"]=>
  string(9) "123456789"
  [0]=>
  string(9) "123456789"
}
