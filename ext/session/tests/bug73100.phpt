--TEST--
Bug #73100 (session_destroy null dereference in ps_files_path_create)
--EXTENSIONS--
session
--INI--
session.save_path=
session.save_handler=files
--FILE--
<?php
ob_start();
var_dump(session_start());
session_module_name("user");
var_dump(session_destroy());

try {
    session_module_name("user");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
===DONE===
--EXPECTF--
bool(true)

Warning: session_module_name(): Session save handler module cannot be changed when a session is active in %s on line %d
bool(true)
session_module_name(): Argument #1 ($module) cannot be "user"
===DONE===
