--TEST--
Bug #70012 (Exception lost with nested finally block)
--FILE--
<?php
try {
	echo "Outer try\n";
	try {
		echo "  Middle try\n";
		throw new Exception();
	} finally {
		echo "  Middle finally\n";
		try {
			echo "    Inner try\n";
		} finally {
			echo "    Inner finally\n";
		}
	}
	echo "Outer shouldn't get here\n";
} catch (Exception $e) {
	echo "Outer catch\n";
} finally {
	echo "Outer finally\n";
}
?>
--EXPECT--
Outer try
  Middle try
  Middle finally
    Inner try
    Inner finally
Outer catch
Outer finally
