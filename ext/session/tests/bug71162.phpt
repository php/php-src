--TEST--
updateTimestamp never called when session data is empty
--INI--
session.use_strict_mode=0
session.save_handler=files
--XFAIL--
Current session module is designed to write empty session always. In addition, current session module only supports SessionHandlerInterface only from PHP 7.0.
--FILE--
<?php
class MySessionHandler extends SessionHandler implements SessionUpdateTimestampHandlerInterface
{
    public function open($path, $sessname) {
        return TRUE;
    }

    public function close() {
        return TRUE;
    }

    public function read($sessid) {
        return '';
    }

    public function write($sessid, $sessdata) {
        echo __FUNCTION__, PHP_EOL;
        return TRUE;
    }

    public function destroy($sessid) {
        return TRUE;
    }

    public function gc($maxlifetime) {
        return TRUE;
    }

	public function create_sid() {
		return sha1(random_bytes(32));
	}

	public function validateId($sid) {
		return TRUE;
	}

    public function updateTimestamp($sessid, $sessdata) {
        echo __FUNCTION__, PHP_EOL;
        return TRUE;
    }
}

ob_start();
$handler = new MySessionHandler();
session_set_save_handler($handler);

session_id(sha1(''));
var_dump(session_id());
var_dump(session_start(['lazy_write'=>1]));
session_commit();

session_id(sha1(''));
var_dump(session_id());
var_dump(session_start(['lazy_write'=>1]));
session_commit();

session_id(sha1(''));
var_dump(session_id());
var_dump(session_start(['lazy_write'=>0]));
session_commit();
?>
--EXPECT--
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
bool(true)
write
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
bool(true)
updateTimestamp
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
bool(true)
write
