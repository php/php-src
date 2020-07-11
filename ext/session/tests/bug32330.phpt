--TEST--
Bug #32330 (session_destroy, "Failed to initialize storage module", custom session handler)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_trans_sid=0
session.use_cookies=1
session.name=sid
session.save_path="{TMP}"
session.gc_probability=1
session.gc_divisor=1
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

function sOpen($path, $name)
{
    echo "open: path = {$path}, name = {$name}\n";
    return TRUE;
}

function sClose()
{
    echo "close\n";
    return TRUE;
}

function sRead($id)
{
    echo "read: id = {$id}\n";
    return '';
}

function sWrite($id, $data)
{
    echo "write: id = {$id}, data = {$data}\n";
    return TRUE;
}

function sDestroy($id)
{
    echo "destroy: id = {$id}\n";
    return TRUE;
}

function sGC($maxlifetime)
{
    echo "gc: maxlifetime = {$maxlifetime}\n";
    return TRUE;
}

session_set_save_handler( 'sOpen', 'sClose', 'sRead', 'sWrite', 'sDestroy', 'sGC' );

// without output buffering, the debug messages will cause all manner of warnings
ob_start();

session_start();
$_SESSION['A'] = 'B';
session_write_close();

session_start();
$_SESSION['C'] = 'D';
session_destroy();

session_start();
$_SESSION['E'] = 'F';
// Don't try to destroy this time!

?>
--EXPECTF--
open: path = %s, name = sid
read: id = %s
gc: maxlifetime = %d
write: id = %s, data = A|s:1:"B";
close
open: path = %s, name = sid
read: id = %s
gc: maxlifetime = %d
destroy: id = %s
close
open: path = %s, name = sid
read: id = %s
gc: maxlifetime = %d
write: id = %s, data = E|s:1:"F";
close
