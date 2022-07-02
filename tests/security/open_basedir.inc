<?php

// This file contains helper functions for testing open_basedir configuration
// Care must be taken with where the directories are created because different
// SAPIs set the working directory differently. So simply creating a directory
// relative to the current working directory like this: mkdir("blah") might
// actually create it in several different places depending on the SAPI..!
//
// Note also depending on the version of php being tested, so the open_basedir
// configuration may or may not be changeable from a script (PHP_INI_SYSTEM).
//
// For this reason we set the open_basedir to . (current directory) and then
// move around to various directories for testing using chdir(). This is NOT
// recommended for production use as . bypasses all semblances of security..!
//
// Although safe mode has been removed in php 6.0, open_basedir is still valid.
//      See http://www.php.net/features.safe-mode for more information

function recursive_delete_directory($directory) {

    // Remove any trailing slash first
    if (substr($directory, -1) == '/') {
        $directory = substr($directory, 0, -1);
    }

    // Make sure the directory is valid
    if (is_dir($directory) == FALSE) {
        return FALSE;
    }

    // Check we can access the directory
    if (is_readable($directory) == FALSE) {
        return FALSE;
    }

    $handle = opendir($directory);

    // Scan through the directory contents
    while (FALSE !== ($item = readdir($handle))) {
        if ($item != '.') {
             if ($item != '..') {
                $path = ($directory.'/'.$item);
                if (is_dir($path) == TRUE) {
                    recursive_delete_directory($path);
                } else {
                    @chmod($path, 0777);
                    unlink($path);
                }
            }
        }
    }

    closedir($handle);
    @chmod($directory, 0777);
    rmdir($directory);

    return TRUE;
}

function create_directories() {
    delete_directories();
    $directory = getcwd();

    var_dump(mkdir($directory."/test"));
    var_dump(mkdir($directory."/test/ok"));
    var_dump(mkdir($directory."/test/bad"));
    file_put_contents($directory."/test/ok/ok.txt", "Hello World!");
    file_put_contents($directory."/test/bad/bad.txt", "Hello World!");
}

function delete_directories() {
    $directory = (getcwd()."/test");
    recursive_delete_directory($directory);
}

function test_open_basedir_error($function) {
    global $savedDirectory;
    var_dump($function("../bad"));
    var_dump($function("../bad/bad.txt"));
    var_dump($function(".."));
    var_dump($function("../"));
    var_dump($function("/"));
    var_dump($function("../bad/."));
    $directory = $savedDirectory;
    var_dump($function($directory."/test/bad/bad.txt"));
    var_dump($function($directory."/test/bad/../bad/bad.txt"));
}

function test_open_basedir_before($function, $change = TRUE) {
    global $savedDirectory;
    echo "*** Testing open_basedir configuration [$function] ***\n";
    $directory = getcwd();
    $savedDirectory = $directory;
    var_dump(chdir($directory));
    create_directories();

    // Optionally change directory
    if ($change == TRUE) {
        var_dump(chdir($directory."/test/ok"));
    }
}

// Delete directories using a --CLEAN-- section!
function test_open_basedir_after($function) {
    echo "*** Finished testing open_basedir configuration [$function] ***\n";
}

// This is used by functions that return an array on success
function test_open_basedir_array($function) {
    global $savedDirectory;

    test_open_basedir_before($function);
    test_open_basedir_error($function);
    var_dump(is_array($function("./../.")));
    var_dump(is_array($function("../ok")));
    var_dump(is_array($function("ok.txt")));
    var_dump(is_array($function("../ok/ok.txt")));
    $directory = $savedDirectory;
    var_dump(is_array($function($directory."/test/ok/ok.txt")));
    var_dump(is_array($function($directory."/test/ok/../ok/ok.txt")));
    test_open_basedir_after($function);
}

function test_open_basedir($function) {
    global $savedDirectory;
    test_open_basedir_before($function);
    test_open_basedir_error($function);
    var_dump($function("./../."));
    var_dump($function("../ok"));
    var_dump($function("ok.txt"));
    var_dump($function("../ok/ok.txt"));
    $directory = $savedDirectory;
    var_dump($function($directory."/test/ok/ok.txt"));
    var_dump($function($directory."/test/ok/../ok/ok.txt"));
    test_open_basedir_after($function);
}

?>
