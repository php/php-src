--TEST--
use-from: method name 'from' remains valid
--FILE--
<?php
namespace DBTest;

class DB {
    public function from($table) {
        echo "from->{$table}\n";
    }
}

$db = new DB();
$db->from('users');

?>
--EXPECT--
from->users
