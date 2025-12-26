--TEST--
Defer with object cleanup
--FILE--
<?php
class Resource {
    public $name;
    public $isOpen = false;

    public function __construct($name) {
        $this->name = $name;
        $this->open();
    }

    public function open() {
        echo "Opening resource: {$this->name}\n";
        $this->isOpen = true;
    }

    public function close() {
        echo "Closing resource: {$this->name}\n";
        $this->isOpen = false;
    }
}

function test() {
    $resource1 = new Resource("Database");

    defer {
        $resource1->close();
    }

    $resource2 = new Resource("File");

    defer {
        $resource2->close();
    }

    echo "Using resources\n";
    throw new Exception("Error occurred");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Opening resource: Database
Opening resource: File
Using resources
Closing resource: File
Closing resource: Database
Caught: Error occurred
