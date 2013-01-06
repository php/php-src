--TEST--
User-space streams: test metadata option
--FILE--
<?php
class test_wrapper {
	function stream_open($path, $mode, $openedpath) {
		return true;
	}
    public function stream_metadata($path, $option, $var) {
		echo "metadata: $path, $option\n";
		if(is_array($var)) {
			echo join(",", $var);
		} else {
			echo $var;
		}
		echo "\n";
		return false;
	}
}

var_dump(stream_wrapper_register('test', 'test_wrapper'));

$fd = fopen("test://foo","r");
touch("test://testdir/touch");
touch("test://testdir/touch", 1);
touch("test://testdir/touch", 1, 2);
chown("test://testdir/chown", "test");
chown("test://testdir/chown", 42);
chgrp("test://testdir/chgrp", "test");
chgrp("test://testdir/chgrp", 42);
chmod("test://testdir/chmod", 0755);
--EXPECT--
bool(true)
metadata: test://testdir/touch, 1

metadata: test://testdir/touch, 1
1,1
metadata: test://testdir/touch, 1
1,2
metadata: test://testdir/chown, 2
test
metadata: test://testdir/chown, 3
42
metadata: test://testdir/chgrp, 4
test
metadata: test://testdir/chgrp, 5
42
metadata: test://testdir/chmod, 6
493