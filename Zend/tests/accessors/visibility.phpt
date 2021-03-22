--TEST--
Accessor visibility
--FILE--
<?php

class Test {
    public $privateSet {
        get { return 42; }
        private set { echo "Called private set()\n"; }
    }

    public $protectedSet {
        get { return 24; }
        protected set { echo "Called protected set()\n"; }
    }

    public $privateGet {
        private get { echo "Called private get()\n"; return 12; }
        set { }
    }

    public $protectedGet {
        protected get { echo "Called protected get()\n"; return 6; }
        set { }
    }

    protected $protectedGetPrivateSet {
        get { echo "Called protected get()\n"; return 42; }
        private set { echo "Called private set()\n"; }
    }

    protected $protectedSetPrivateGet {
        private get { echo "Called private get()\n"; return 3; }
        set { echo "Called protecated set()\n"; }
    }

    public function testPrivate() {
        $this->privateSet = 42;
        $this->privateGet;
        $this->protectedSet = 42;
        $this->protectedGet;
        $this->protectedGetPrivateSet = 42;
        $this->protectedSetPrivateGet;
        $this->protectedGetPrivateSet;
        $this->protectedSetPrivateGet = 42;
    }
}

class Test2 extends Test {
    public function testProtected() {
        try {
            $this->privateSet = 42;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            $this->privateGet;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        $this->protectedSet = 42;
        $this->protectedGet;
        try {
            $this->protectedGetPrivateSet = 42;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            $this->protectedSetPrivateGet;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        $this->protectedGetPrivateSet;
        $this->protectedSetPrivateGet = 42;
    }
}

$test = new Test2;

var_dump($test->privateSet);
$test->privateGet = 42;
var_dump($test->protectedSet);
$test->protectedGet = 42;

$test->testPrivate();
echo "\n";
$test->testProtected();
echo "\n";

try {
    $test->privateSet = 42;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->privateGet;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->protectedSet = 42;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->protectedGet;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->protectedGetPrivateSet = 42;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->protectedSetPrivateGet;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->protectedGetPrivateSet;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->protectedSetPrivateGet = 42;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
int(24)
Called private set()
Called private get()
Called protected set()
Called protected get()
Called private set()
Called private get()
Called protected get()
Called protecated set()

Call to private accessor Test::$privateSet::set() from scope Test2
Call to private accessor Test::$privateGet::get() from scope Test2
Called protected set()
Called protected get()
Call to private accessor Test::$protectedGetPrivateSet::set() from scope Test2
Call to private accessor Test::$protectedSetPrivateGet::get() from scope Test2
Called protected get()
Called protecated set()

Call to private accessor Test::$privateSet::set() from global scope
Call to private accessor Test::$privateGet::get() from global scope
Call to protected accessor Test::$protectedSet::set() from global scope
Call to protected accessor Test::$protectedGet::get() from global scope
Cannot access protected property Test2::$protectedGetPrivateSet
Cannot access protected property Test2::$protectedSetPrivateGet
Cannot access protected property Test2::$protectedGetPrivateSet
Cannot access protected property Test2::$protectedSetPrivateGet
