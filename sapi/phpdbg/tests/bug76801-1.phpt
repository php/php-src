--TEST--
get_included_files() tests
--FILE--
<?php

file_put_contents(__DIR__ . '/bug76801-2.php', '<?php echo __LINE__;');
require(__DIR__."/bug76801-2.php");
file_put_contents(__DIR__ . '/bug76801-2.php', '<?php');

echo "Done\n";
?>
--EXPECTF--
1Done
--CLEAN--
<?php
@unlink(__DIR__.'/bug76801-2.php');
