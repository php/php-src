--TEST--
Bug #50558 - Broken object model when extending tidy
--EXTENSIONS--
tidy
--FILE--
<?php
class MyTidy extends tidy
{
            // foo
}

function doSomething(MyTidy $o)
{
            var_dump($o);
}

$o = new MyTidy();
var_dump($o instanceof MyTidy);
doSomething($o);
?>
--EXPECTF--
bool(true)
object(MyTidy)#%d (%d) {
  ["errorBuffer"]=>
  NULL
  ["value"]=>
  NULL
}
