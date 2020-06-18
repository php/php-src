--TEST--
Bug #31102 (Exception not handled when thrown inside autoloader)
--FILE--
<?php

$test = 0;

spl_autoload_register(function ($class) {
    global $test;

    echo __METHOD__ . "($class,$test)\n";
    switch($test)
    {
    case 1:
        eval("class $class { function __construct(){throw new Exception('$class::__construct');}}");
        return;
    case 2:
        eval("class $class { function __construct(){throw new Exception('$class::__construct');}}");
        throw new Exception(__METHOD__);
        return;
    case 3:
        return;
    }
});

while($test++ < 5)
{
    try
    {
        eval("\$bug = new Test$test();");
    }
    catch (Exception $e)
    {
        echo "Caught: " . $e->getMessage() . "\n";
    }
}
?>
===DONE===
--EXPECTF--
{closure}(Test1,1)
Caught: Test1::__construct
{closure}(Test2,2)
Caught: {closure}
{closure}(Test3,3)

Fatal error: Uncaught Error: Class "Test3" not found in %s:%d
Stack trace:
#0 %s(%d): eval()
#1 {main}
  thrown in %sbug31102.php(%d) : eval()'d code on line 1
