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

?>
--EXPECTF--
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
