--TEST--
082: bracketed namespace with closing tag
--FILE--
<?php
namespace foo {
}
namespace ok {
echo "ok\n";
}
?>
--EXPECT--
ok
