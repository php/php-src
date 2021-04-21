--TEST--
Fibers created during cleanup
--FILE--
<?php

$fibers = [];
for ($i = 0; $i < 5; $i++) {
    $fibers[$i] = new Fiber(function() {
        try {
            Fiber::suspend();
        } finally {
            echo "finally\n";
            $fiber2 = new Fiber(function() {
                echo "new\n";
                try {
                    Fiber::suspend();
                } finally {
                    echo "new finally\n";
                }
            });
            $fiber2->start();
        }
    });
    $fibers[$i]->start();
}

?>
--EXPECT--
finally
new
new finally
finally
new
new finally
finally
new
new finally
finally
new
new finally
finally
new
new finally
