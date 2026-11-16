--TEST--
Session files handler truncates repeated writes to the current data length
--EXTENSIONS--
session
--INI--
session.save_handler=files
session.save_path={PWD}
session.use_cookies=0
session.cache_limiter=
session.gc_probability=0
--FILE--
<?php
$id = 'session-files-repeated-write';
$file = __DIR__ . '/sess_' . $id;
$handler = new SessionHandler();
session_set_save_handler($handler);
$values = ['longer data', 'short', 'SHORT', 'much longer data', ''];

for ($count = 1; $count <= count($values); $count++) {
    file_put_contents($file, '');
    session_id($id);
    session_start();
    foreach (array_slice($values, 0, $count) as $value) {
        var_dump($handler->write($id, $value));
    }
    session_abort();
    var_dump(file_get_contents($file));
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/sess_session-files-repeated-write');
?>
--EXPECT--
bool(true)
string(11) "longer data"
bool(true)
bool(true)
string(5) "short"
bool(true)
bool(true)
bool(true)
string(5) "SHORT"
bool(true)
bool(true)
bool(true)
bool(true)
string(16) "much longer data"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(0) ""
