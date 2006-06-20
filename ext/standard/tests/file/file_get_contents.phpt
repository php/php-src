--TEST--
file_get_contents() leaks on empty file
--FILE--
<?php
    $tmpfname = tempnam("/tmp", "emptyfile");
    echo file_get_contents($tmpfname), "done.";
?>
--EXPECT--
done.
