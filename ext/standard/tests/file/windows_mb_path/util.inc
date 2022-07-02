<?php

function get_active_cp($kind = "")
{
    if (version_compare(PHP_VERSION, '7.1', '<')) {
        $s = exec("chcp");
        preg_match(",.*: (\d+),", $s, $m);

        return $m[1];
    } else {
        return sapi_windows_cp_get($kind);
    }
}

function set_active_cp($cp, $echo = true)
{
    if (version_compare(PHP_VERSION, '7.1', '<')) {
        $ret = exec("chcp $cp");
    } else {
        if (!sapi_windows_cp_set($cp)) {
            echo "Failed to set cp $cp\n";
            return;
        }

        if ($echo) echo "Active code page: ", get_active_cp(), "\n";
    }
}

function get_basename_with_cp($path, $cp, $echo = true)
{
    $old_cp = get_active_cp();
    set_active_cp($cp, $echo);

    if ($echo) echo "getting basename of $path\n";

    $cmd = "powershell -command \"Get-Item -Path '$path' | Format-Table -HideTableHeaders Name\"";
    $out = trim(shell_exec($cmd));

    if ($echo) var_dump($out, $out == basename($path));
    if ($echo) var_dump(realpath($path));

    set_active_cp($old_cp, $echo);

    return $out;
}

function skip_if_wrong_cp($cp, $kind = "")
{
    if (get_active_cp($kind) != $cp) {
        die("skip this test expect codepage $cp");
    }
}

function skip_if_no_required_exts()
{
    $exts = func_get_args();
    $exts[] = "iconv";

    foreach ($exts as $ext) {
        if (!extension_loaded($ext)) {
            die("skip $ext is not loaded");
        }
    }
}

function skip_if_not_win()
{
    if(substr(PHP_OS, 0, 3) != 'WIN' ) {
        die('skip windows only test');
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

function create_data($id, $item = "", $cp = 65001, $utf8 = true)
{
    if ($utf8) {
        /* Keep this file ASCII, so zend.multibyte related stuff can be tasted as well. */
        include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util_utf8.inc";
        return create_data_from_utf8($id, $item, $cp);
    } else {

        $prefix = dirname(__FILE__) . DIRECTORY_SEPARATOR . $id;

        if (!is_dir($prefix)) {
            mkdir($prefix);
        }

        if (0 === strpos($id, "dir")) {
            create_verify_dir($prefix, $item, $cp);
        } else if (0 === strpos($id, "file")) {
            /* a bit unhandy, but content can be put from outside, if needed */
            create_verify_file($prefix, $item, "dummy content", $cp);
        } else {
            echo "Item has either to start with \"dir\" or \"file\"";
        }
    }

    return $prefix;
}
