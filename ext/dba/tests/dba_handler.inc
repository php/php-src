<?php

echo "database handler: $handler\n";

do {
    if (($db_file = dba_open($db_filename, 'n'.$lock_flag, $handler))!==FALSE) {
        dba_insert("key1", "Content String 1", $db_file);
        dba_insert("key2", "Content String 2", $db_file);
        dba_insert("key3", "Third Content String", $db_file);
        dba_insert("key4", "Another Content String", $db_file);
        dba_insert("key5", "The last content string", $db_file);
        if ($handler != 'cdb') {
            dba_delete("key3", $db_file);
            dba_delete("key1", $db_file);
        } else {
            dba_close($db_file);
            if (($db_file = dba_open($db_filename, 'r'.$lock_flag, $handler))===FALSE) {
                echo "Error reopening database\n";
            }
        }
        $a = dba_firstkey($db_file);
        $i=0;
        while($a) {
            $a = dba_nextkey($db_file);
            $i++;
        }
        echo $i;
        for ($i=1; $i<6; $i++) {
            echo dba_exists("key$i", $db_file) ? "Y" : "N";
        }
        echo "\n";
        echo dba_fetch("key2", $db_file)."\n";
        if ($handler != 'cdb') {
            dba_replace("key2", "Content 2 replaced", $db_file);
            echo dba_fetch("key2", $db_file)."\n";
        }
        dba_close($db_file);
    } else {
        echo "Error creating database\n";
    }
    if ($handler != 'cdb') {
        $db_writer = dba_open($db_filename, 'c'.$lock_flag, $handler);
        if (($dba_reader = @dba_open($db_filename, 'r'.$lock_flag.'t', $handler))===false) {
            echo "Read during write: not allowed\n";
        } else {
            echo "Read during write: allowed\n";
        }
        if ($db_writer!==FALSE) {
            if (dba_insert("key number 6", "The 6th value", $db_writer)) {
                echo '"key number 6" written' . "\n";
            } else {
                echo 'Failed to write "key number 6"' . "\n";
            }
            if (dba_insert("key number 6", "The 6th value inserted again would be an error", $db_writer)) {
                echo '"key number 6" written 2nd time' . "\n";
            } else {
                echo 'Failed to write "key number 6" 2nd time' . "\n";
            }
            dba_replace("key2", "Content 2 replaced 2nd time", $db_writer);
            dba_delete("key4", $db_writer);
            echo dba_fetch("key2", $db_writer)."\n";
            echo dba_fetch("key number 6", $db_writer)."\n";
            dba_close($db_writer); // when the writer is open at least db3 would fail because of buffered io.
        } else {
            die("Error reopening database\n");
        }
    }
    if (($db_file = dba_open($db_filename, 'r'.$lock_flag, $handler))!==FALSE) {
        $key = dba_firstkey($db_file);
        $res = array();
        while($key) {
            $res[$key] = dba_fetch($key, $db_file);
            $key = dba_nextkey($db_file);
        }
        ksort($res);
        var_dump($res);
        dba_close($db_file);
    } else {
        echo "Error reading database\n";
    }
    if (!empty($dba_reader)) {
        dba_close($dba_reader);
    }
    if (($db_file = dba_popen($db_filename, 'r'.($lock_flag==''?'':'-'), $handler))!==FALSE) {
        if ($handler == 'dbm' || $handler == "tcadb") {
            dba_close($db_file);
        }
    }

    if ($lock_flag == '') {
        break;
    } else {
        echo "--NO-LOCK--\n";
        $lock_flag = '';
    }
} while(1);

?>
