--TEST--
tests Fiber for exception
--FILE--
<?php
function foo()
{
    Fiber::yield();
    throw new Exception();
}

function bar()
{
    Fiber::yield();
    foo();
}

$f = new Fiber('bar');

$f->resume();
$f->resume();

try {
    $f->resume();
} catch (\Exception $e) {
    echo $e->getTraceAsString();
}
?>
--EXPECTF--
#0 %s017-exception-on-resume.php(11): foo()
#1 (0): bar()
#2 {main}
