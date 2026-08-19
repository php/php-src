--TEST--
Namespace declarations work regardless of casing
--FILE--
<?php
namespace MyApp\Service {
    class UserService {}
}

namespace MYAPP\Service {
    class OtherService {}
}

namespace myapp\service {
    class ThirdService {}
}

namespace MyApp\Service {
    class AnotherService {}
    echo "done\n";
}
?>
--EXPECT--
done
