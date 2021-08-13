<?php
$save_path = ini_get("session.save_path");
if ($save_path) {
    if (!file_exists($save_path)) {
        die("skip Session save_path doesn't exist");
    }

    if ($save_path && !@is_writable($save_path)) {
        if (($p = strpos($save_path, ';')) !== false) {
            $save_path = substr($save_path, ++$p);
        }
        if (!@is_writable($save_path)) {
            die("skip session.save_path $save_path is not writable\n");
        }
    }
}
?>
