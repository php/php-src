--TEST--
exit() inside a task terminates the script
--FILE--
<?php
async function quit() {
    await Async\delay(0);
    echo "exiting\n";
    exit("bye\n");
}
await quit();
echo "unreachable\n";
?>
--EXPECT--
exiting
bye
