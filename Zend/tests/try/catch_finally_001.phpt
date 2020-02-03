--TEST--
Try catch finally (basic test)
--FILE--
<?php

function foo($throw = false)
{
    try {
        echo "try\n";
        if ($throw) {
            throw new Exception("ex");
        }
    } catch (Exception $e) {
        echo "catch\n";
    } finally {
        echo "finally\n";
    }

    echo "end\n";
}

foo();
echo "\n";
foo(true);

?>
--EXPECT--
try
finally
end

try
catch
finally
end
