--TEST--
file_get_contents() leaks on empty file
--FILE--
<?php
    $tmpfname = tempnam(sys_get_temp_dir(), "emptyfile");
    echo file_get_contents($tmpfname), "done.";
?>
--EXPECT--
done.
