--TEST--
Namespace end resets seen class symbols
--FILE--
<?php

namespace {
    class C {}
    var_dump(new C);
}

namespace Ns {
    class C {}
    var_dump(new C);
}

namespace {
    use Ns\C;
    var_dump(new C);
}

namespace Ns {
    use C;
    var_dump(new C);
}

namespace {
    var_dump(new C);
}

namespace Ns {
    var_dump(new C);
}

namespace {
    use C;
    var_dump(new C);
}

namespace Ns {
    use Ns\C;
    var_dump(new C);
}

?>
--EXPECTF--
Warning: The use statement with non-compound name 'C' has no effect in %s on line 32
object(C)#%d (0) {
}
object(Ns\C)#1 (0) {
}
object(Ns\C)#1 (0) {
}
object(C)#%d (0) {
}
object(C)#%d (0) {
}
object(Ns\C)#1 (0) {
}
object(C)#%d (0) {
}
object(Ns\C)#1 (0) {
}
