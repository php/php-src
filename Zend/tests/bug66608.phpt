--TEST--
Bug #66608 (Incorrect behavior with nested "finally" blocks)
--FILE--
<?php
function bar() {
	try {
		echo "1\n";
	} finally {
		try {
			throw new Exception ("");
		} catch (Exception $ab) {
			echo "2\n";
		} finally {
			try {
			} finally {
				echo "3\n";
				try  {
				} finally {
				}
				echo "4\n";
			}
		}
		echo "5\n";
		try {
		} finally {
			echo "6\n";
		}
	}
	echo "7\n";
}
bar();
--EXPECT--
1
2
3
4
5
6
7
