--TEST--
Pending tasks are destroyed at shutdown and run their finally blocks
--FILE--
<?php
async function pending() {
    try {
        echo "waiting\n";
        await Async\delay(10);
        echo "unreachable\n";
    } finally {
        echo "finally\n";
    }
}
$f = pending();
echo "end of script\n";
?>
--EXPECT--
waiting
end of script
finally
