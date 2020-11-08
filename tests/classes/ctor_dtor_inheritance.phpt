--TEST--
ZE2 A derived class can use the inherited constructor/destructor
--FILE--
<?php

// This test checks for:
// - inherited constructors/destructors are not called automatically
// - base classes know about derived properties in constructor/destructor
// - base class constructors/destructors know the instantiated class name

class base {
    public $name;

    function __construct() {
        echo __CLASS__ . "::" . __FUNCTION__ . "\n";
        $this->name = 'base';
        print_r($this);
    }

    function __destruct() {
        echo __CLASS__ . "::" . __FUNCTION__ . "\n";
        print_r($this);
    }
}

class derived extends base {
    public $other;

    function __construct() {
        $this->name = 'init';
        $this->other = 'other';
        print_r($this);
        parent::__construct();
        echo __CLASS__ . "::" . __FUNCTION__ . "\n";
        $this->name = 'derived';
        print_r($this);
    }

    function __destruct() {
        parent::__destruct();
        echo __CLASS__ . "::" . __FUNCTION__ . "\n";
        print_r($this);
    }
}

echo "Testing class base\n";
$t = new base();
unset($t);
echo "Testing class derived\n";
$t = new derived();
unset($t);

echo "Done\n";
?>
--EXPECT--
Testing class base
base::__construct
base Object
(
    [name] => base
)
base::__destruct
base Object
(
    [name] => base
)
Testing class derived
derived Object
(
    [other] => other
    [name] => init
)
base::__construct
derived Object
(
    [other] => other
    [name] => base
)
derived::__construct
derived Object
(
    [other] => other
    [name] => derived
)
base::__destruct
derived Object
(
    [other] => other
    [name] => derived
)
derived::__destruct
derived Object
(
    [other] => other
    [name] => derived
)
Done
