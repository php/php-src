--TEST--
Lazy objects: RFC example 009
--FILE--
<?php

class Connection {
    public $prop;
    public function __construct() {
        $this->connect();
    }
    public function __destruct() {
        var_dump(__METHOD__);
        $this->close();
    }
    public function connect() {
    }
    public function close() {
    }
}

$connection = new Connection();

$reflector = new ReflectionClass(Connection::class);

print "Reset non-lazy\n";
// Calls destructor
$reflector->resetAsLazyGhost($connection, function () {
    var_dump("initialization");
});

print "Release non-initialized\n";
$connection = null; // Does not call destructor (object is not initialized)

?>
==DONE==
--EXPECT--
Reset non-lazy
string(22) "Connection::__destruct"
Release non-initialized
==DONE==
