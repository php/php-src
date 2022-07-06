<?php
function dump_entries_name($z) {
    for($i=0; $i<$z->numFiles; $i++) {
        $sb = $z->statIndex($i);
        echo $i . ' ' . $sb['name'] . "\n";
    }
}

function verify_entries($zip, $entries = []) {
    $verified = true;
    $found    = [];

    for ($index = 0; $index < $zip->numFiles; $index++) {
        $stat = $zip->statIndex($index);

        if (!in_array($stat["name"], $entries)) {
            $verified = false;
        }

        $found[] = $stat["name"];
    }

    if (!$verified) {
        var_dump($found);
    }

    return $verified;
}

/* recursively remove a directory */
function rmdir_rf($dir) {
    if ($handle = opendir($dir)) {
        while (false !== ($item = readdir($handle))) {
            if ($item != "." && $item != "..") {
                if (is_dir($dir . '/' . $item)) {
                    rmdir_rf($dir . '/' . $item);
                } else {
                    unlink($dir . '/' . $item);
                }
            }
        }
        closedir($handle);
        rmdir($dir);
    }
}
