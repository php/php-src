--TEST--
Namespace end resets seen function symbols
--FILE--
<?php

namespace {
    function f() {
        echo __FUNCTION__, "\n";
    }
    f();
}

namespace Ns {
    function f() {
        echo __FUNCTION__, "\n";
    }
    f();
}

namespace {
    use function Ns\f;
    f();
}

namespace Ns {
    use function f;
    f();
}

namespace {
    f();
}

namespace Ns {
    f();
}

namespace {
    use function f;
    f();
}

namespace Ns {
    use function Ns\f;
    f();
}

?>
--EXPECTF--
Warning: The use statement with non-compound name 'f' has no effect in %s on line 36
f
Ns\f
Ns\f
f
f
Ns\f
f
Ns\f
