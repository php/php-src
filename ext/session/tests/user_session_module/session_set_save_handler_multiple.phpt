--TEST--
session_set_save_handler(): optional closures not set again in second call
--INI--
session.use_strict_mode=1
session.name=PHPSESSID
session.serialize_handler=php_serialize
--EXTENSIONS--
session
--FILE--
<?php

$data = [];

ob_start();
function open($path, $name): bool {
    echo 'Open', "\n";
    return true;
}
function create_sid(): string {
    echo 'Create SID OLD', "\n";
    return 'OLD_ID';
}
function read($key): string|false {
    echo 'Read', "\n";
    global $data;
    return serialize($data);
}
function write($key, $val): bool {
    echo 'Write', "\n";
    global $data;
    $data[$key] = $val;
    return true;
}
function close(): bool {
    echo 'Close', "\n";
    return true;
}
function destroy($id): bool {
    echo 'Destroy', "\n";
    return true;
}
function gc($lifetime): bool {
    return true;
}
function createSid(): string {
    echo 'Create SID NEW', "\n";
    return 'NEW_ID';
}
function validateId($key): bool {
    echo 'Validate ID', "\n";
    return true;
}
function updateTimestamp($key, $data): bool {
    echo 'Update Timestamp', "\n";
    return true;
}

session_set_save_handler('open', 'close', 'read', 'write', 'destroy', 'gc', 'create_sid', 'validateId', 'updateTimestamp');
session_start();

$_SESSION['foo'] = "hello";

session_write_close();
session_unset();

echo "New handlers:\n";
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
session_start();
var_dump($_SESSION);
$_SESSION['bar'] = "world";
session_write_close();

ob_end_flush();
?>
--EXPECTF--
Deprecated: Calling session_set_save_handler() with more than 2 arguments is deprecated in %s on line %d
Open
Create SID OLD
Read
Write
Close
New handlers:

Deprecated: Calling session_set_save_handler() with more than 2 arguments is deprecated in %s on line %d
Open
Validate ID
Read
array(1) {
  ["OLD_ID"]=>
  string(28) "a:1:{s:3:"foo";s:5:"hello";}"
}
Write
Close
