--TEST--
User-space streams: stream_lock()
--FILE--
<?php
class test_wrapper_base {
	public $mode;
	function stream_open($path, $mode, $openedpath) {
		return true;
	}
	function stream_eof() {
		return false;
	}
}
class test_wrapper extends test_wrapper_base {
	function stream_lock($mode) {
		$this->mode = $mode;
	}
}
function test($name, $fd, $mode) {
	echo "------ $name: -------\n";
	flock($fd, $mode);
	$data = stream_get_meta_data($fd);
	var_dump($data['wrapper_data']->mode === $mode);
}

var_dump(stream_wrapper_register('test', 'test_wrapper'));
var_dump(stream_wrapper_register('test2', 'test_wrapper_base'));

$fd = fopen("test://foo","r");
$fd2 = fopen("test2://foo","r");

test("stream_lock not implemented", $fd2, LOCK_EX);

foreach(array("LOCK_SH","LOCK_EX","LOCK_UN") as $mode) {
	test("fclock($mode)", $fd, constant($mode));
	test("fclock($mode|LOCK_NB)", $fd, constant($mode)|LOCK_NB);
}

?>
--EXPECTF--
bool(true)
bool(true)
------ stream_lock not implemented: -------

Warning: flock(): test_wrapper_base::stream_lock is not implemented! in %s
bool(false)
------ fclock(LOCK_SH): -------
bool(true)
------ fclock(LOCK_SH|LOCK_NB): -------
bool(true)
------ fclock(LOCK_EX): -------
bool(true)
------ fclock(LOCK_EX|LOCK_NB): -------
bool(true)
------ fclock(LOCK_UN): -------
bool(true)
------ fclock(LOCK_UN|LOCK_NB): -------
bool(true)
