<?php

function get_basename($path, $echo = true)
{
    if ($echo) echo "getting basename of $path\n";

    $cmd = "powershell -command \"Get-Item -Path '$path' | Format-Table -HideTableHeaders Name\"";
    $out = trim(shell_exec($cmd));

    if ($echo) var_dump($out, $out == basename($path));
    if ($echo) var_dump(realpath($path));

    return $out;
}

function get_basename_with_cp($path, $cp, $echo = true)
{
    $old_cp = sapi_windows_cp_get();
    sapi_windows_cp_set($cp);
    if ($echo) echo "Active code page: ", sapi_windows_cp_get(), "\n";

    $out = get_basename($path, $echo);

    sapi_windows_cp_set($old_cp);
    if ($echo) echo "Active code page: ", sapi_windows_cp_get(), "\n";

    return $out;
}

function skip_if_wrong_cp($cp, $kind = "")
{
    if (sapi_windows_cp_get($kind) != $cp) {
        die("skip this test expect codepage $cp");
    }
}

function create_verify_file($prefix, $basename, $content = "", $cp = 65001)
{
    $full = $prefix . DIRECTORY_SEPARATOR . $basename;

    if (!touch($full)) {
        echo "failed to touch create $full\n";
        return;
    }

    $now = get_basename_with_cp($full, $cp, false);
    if ($now !== $basename) {
        echo "expected '$basename', got '$now'\n";
        return;
    }

    if ($content) {
        file_put_contents($full, $content);
    }
}

function create_verify_dir($prefix, $basename, $cp = 65001)
{
    $full = $prefix . DIRECTORY_SEPARATOR . $basename;

    if (!mkdir($full)) {
        echo "failed to create dir '$full'\n";
        return;
    }

    $now = get_basename_with_cp($full, $cp, false);
    if ($now !== $basename) {
        echo "expected '$basename', got '$now'\n";
    }
}

function remove_data($id, $dir = NULL)
{
    if (!$dir) {
        $dir = dirname(__FILE__) . DIRECTORY_SEPARATOR . $id;
    }

    if (is_dir($dir)) {
        $objects = scandir($dir);
        foreach ($objects as $object) {
            if ($object != "." && $object != "..") {
                if (filetype($dir . DIRECTORY_SEPARATOR . $object) == "dir")
                    remove_data($id, $dir . DIRECTORY_SEPARATOR . $object);
                else
                    unlink($dir . DIRECTORY_SEPARATOR . $object);
            }
        }
        reset($objects);
        rmdir($dir);
    }
}

/* Keep this file ASCII, so zend.multibyte related stuff can be tested as well. */
include __DIR__ . DIRECTORY_SEPARATOR . "util_utf8.inc";
function create_data($id, $item = "", $cp = 65001)
{
    return create_data_from_utf8($id, $item, $cp);
}
