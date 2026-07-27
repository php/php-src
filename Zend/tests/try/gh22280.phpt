--TEST--
GH-22280: goto to label before try/finally after try/catch
--FILE--
<?php
goto d;
try {
} catch (Throwable) {
}
d: try {
	echo "try\n";
} finally {
	echo "finally\n";
}
echo "done\n";
?>
--EXPECT--
try
finally
done
