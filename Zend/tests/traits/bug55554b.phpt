--TEST--
Bug #55137 (Legacy constructor not registered for class)
--FILE--
<?php

trait TConstructor {
    public function foo() {
        echo "foo executed\n";
    }
    public function bar() {
        echo "bar executed\n";
    }
}

class OverridingIsSilent1 {
    use TConstructor {
	    foo as __construct;
	}

	public function __construct() {
	    echo "OverridingIsSilent1 __construct\n";
	}
}

$o = new OverridingIsSilent1;

class OverridingIsSilent2 {
    use TConstructor {
	    foo as OverridingIsSilent2;
	}

	public function OverridingIsSilent2() {
	    echo "OverridingIsSilent2 OverridingIsSilent2\n";
	}
}

$o = new OverridingIsSilent2;

class ReportCollision {
	use TConstructor {
	    bar as ReportCollision;
	    foo as __construct;
	}
}


echo "ReportCollision: ";
$o = new ReportCollision;
--EXPECTF--
OverridingIsSilent1 __construct

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; OverridingIsSilent2 has a deprecated constructor in %s on line %d
OverridingIsSilent2 OverridingIsSilent2

Fatal error: ReportCollision has colliding constructor definitions coming from traits in %s on line %d
