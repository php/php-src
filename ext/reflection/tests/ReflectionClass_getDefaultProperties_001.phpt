--TEST--
ReflectionClass::getDefaultProperties(), ReflectionClass::getStaticProperties()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php


class A {
    static public $statPubC = "stat pubC in A";
    static protected $statProtC = "stat protC in A";
    static private $statPrivC = "stat privC in A";

    static public $statPubA = "stat pubA in A";
    static protected $statProtA = "stat protA in A";
    static private $statPrivA = "stat privA in A";

    public $pubC = "pubC in A";
    protected $protC = "protC in A";
    private $privC = "privC in A";

    public $pubA = "pubA in A";
    protected $protA = "protA in A";
    private $privA = "privA in A";
}

class B extends A {
    static public $statPubC = "stat pubC in B";
    static protected $statProtC = "stat protC in B";
    static private $statPrivC = "stat privC in B";

    static public $statPubB = "stat pubB in B";
    static protected $statProtB = "stat protB in B";
    static private $statPrivB = "stat privB in B";

    public $pubC = "pubC in B";
    protected $protC = "protC in B";
    private $privC = "privC in B";

    public $pubB = "pubB in B";
    protected $protB = "protB in B";
    private $privB = "privB in B";
}

class C extends B {
    static public $statPubC = "stat pubC in C";
    static protected $statProtC = "stat protC in C";
    static private $statPrivC = "stat privC in C";

    public $pubC = "pubC in C";
    protected $protC = "protC in C";
    private $privC = "privC in C";
}

class X {
    static public $statPubC = "stat pubC in X";
    static protected $statProtC = "stat protC in X";
    static private $statPrivC = "stat privC in X";

    public $pubC = "pubC in X";
    protected $protC = "protC in X";
    private $privC = "privC in X";
}

$classes = array('A', 'B', 'C', 'X');
foreach ($classes as $class) {
    $rc = new ReflectionClass($class);
    echo "\n\n---- Static properties in $class ----\n";
    print_r($rc->getStaticProperties());
    echo "\n\n---- Default properties in $class ----\n";
    print_r($rc->getDefaultProperties());
}

?>
--EXPECT--
---- Static properties in A ----
Array
(
    [statPubC] => stat pubC in A
    [statProtC] => stat protC in A
    [statPrivC] => stat privC in A
    [statPubA] => stat pubA in A
    [statProtA] => stat protA in A
    [statPrivA] => stat privA in A
)


---- Default properties in A ----
Array
(
    [statPubC] => stat pubC in A
    [statProtC] => stat protC in A
    [statPrivC] => stat privC in A
    [statPubA] => stat pubA in A
    [statProtA] => stat protA in A
    [statPrivA] => stat privA in A
    [pubC] => pubC in A
    [protC] => protC in A
    [privC] => privC in A
    [pubA] => pubA in A
    [protA] => protA in A
    [privA] => privA in A
)


---- Static properties in B ----
Array
(
    [statPubC] => stat pubC in B
    [statProtC] => stat protC in B
    [statPrivC] => stat privC in B
    [statPubB] => stat pubB in B
    [statProtB] => stat protB in B
    [statPrivB] => stat privB in B
    [statPubA] => stat pubA in A
    [statProtA] => stat protA in A
)


---- Default properties in B ----
Array
(
    [statPubC] => stat pubC in B
    [statProtC] => stat protC in B
    [statPrivC] => stat privC in B
    [statPubB] => stat pubB in B
    [statProtB] => stat protB in B
    [statPrivB] => stat privB in B
    [statPubA] => stat pubA in A
    [statProtA] => stat protA in A
    [pubC] => pubC in B
    [protC] => protC in B
    [privC] => privC in B
    [pubB] => pubB in B
    [protB] => protB in B
    [privB] => privB in B
    [pubA] => pubA in A
    [protA] => protA in A
)


---- Static properties in C ----
Array
(
    [statPubC] => stat pubC in C
    [statProtC] => stat protC in C
    [statPrivC] => stat privC in C
    [statPubB] => stat pubB in B
    [statProtB] => stat protB in B
    [statPubA] => stat pubA in A
    [statProtA] => stat protA in A
)


---- Default properties in C ----
Array
(
    [statPubC] => stat pubC in C
    [statProtC] => stat protC in C
    [statPrivC] => stat privC in C
    [statPubB] => stat pubB in B
    [statProtB] => stat protB in B
    [statPubA] => stat pubA in A
    [statProtA] => stat protA in A
    [pubC] => pubC in C
    [protC] => protC in C
    [privC] => privC in C
    [pubB] => pubB in B
    [protB] => protB in B
    [pubA] => pubA in A
    [protA] => protA in A
)


---- Static properties in X ----
Array
(
    [statPubC] => stat pubC in X
    [statProtC] => stat protC in X
    [statPrivC] => stat privC in X
)


---- Default properties in X ----
Array
(
    [statPubC] => stat pubC in X
    [statProtC] => stat protC in X
    [statPrivC] => stat privC in X
    [pubC] => pubC in X
    [protC] => protC in X
    [privC] => privC in X
)
