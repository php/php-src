--TEST--
Bug #37864 (file_get_contents() leaks on empty file)
--FILE--
<?php
	$tmpfname = tempnam("/tmp", "emptyfile");
	var_dump(file_get_contents($tmpfname));
	echo "done.\n";
	unlink($tmpfname);
?>
--EXPECT--
string(0) ""
done.
