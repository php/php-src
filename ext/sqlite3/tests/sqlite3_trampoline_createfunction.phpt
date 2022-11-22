--TEST--
SQLite3::createFunction trampoline callback
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return strtoupper($arguments[0]);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'strtoupper'];
var_dump($db->createfunction('strtoupper', $callback));
var_dump($db->querySingle('SELECT strtoupper("test")'));
var_dump($db->querySingle('SELECT strtoupper("test")'));

?>
--EXPECT--
bool(true)
Trampoline for strtoupper
string(4) "TEST"
Trampoline for strtoupper
string(4) "TEST"
