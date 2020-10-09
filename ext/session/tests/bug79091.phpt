--TEST--
Bug #79091 (heap use-after-free in session_create_id())
--SKIPIF--
<?php
if (!extension_loaded('session')) die('skip session extension not available');
?>
--FILE--
<?php
class MySessionHandler implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface
{
    public function close()
    {
        return true;
    }

    public function destroy($session_id)
    {
        return true;
    }

    public function gc($maxlifetime)
    {
        return true;
    }

    public function open($save_path, $session_name)
    {
        return true;
    }

    public function read($session_id)
    {
        return '';
    }

    public function write($session_id, $session_data)
    {
        return true;
    }

    public function create_sid()
    {
        return uniqid();
    }

    public function updateTimestamp($key, $val)
    {
        return true;
    }

    public function validateId($key)
    {
        return true;
    }
}

ob_start();
var_dump(session_set_save_handler(new MySessionHandler()));
var_dump(session_start());
ob_flush();
session_create_id();
?>
--EXPECTF--
bool(true)
bool(true)

Warning: session_create_id(): Failed to create new ID in %s on line %d
