--TEST--
Bug #43183 ("use" of the same class in difference scripts results in a fatal error)
--FILE--
<?php
$file1 = <<<'EOF'
namespace Test;
class Helper {}
EOF;

$file2 = <<<'EOF'
namespace Test;
use Test\Helper;
class Other {}
EOF;

eval($file1);
eval($file2);

var_dump(new Test\Helper);

--EXPECTF--
object(Test\Helper)#%d (0) {
}
