<?php

function check_skip_any() {
    if (dba_handlers() === []) {
        die('skip no handlers installed');
    }
    if (dba_handlers() === ['cdb']) {
        die('skip only cdb installed which is not suitable');
    }
    if (dba_handlers() === ['cdb_make']) {
        die('skip only cdb_make installed which is not suitable');
    }
}

function check_skip(string $handler) {
    $handlers = dba_handlers();
    if ($handlers === []) {
        die('skip no handlers installed');
    }
    if (!in_array($handler, $handlers)) {
        $HND = strtoupper($handler);
        die("skip $HND handler not available");
    }
}

function get_any_handler(): string {
    foreach (dba_handlers() as $handler) {
        // Those are weird
        if ($handler !== 'cdb' && $handler !== 'cdb_make' && $handler !== 'inifile') {
            echo 'Using handler: "', $handler, '"', \PHP_EOL;
            return $handler;
        }
    }
    return 'should_not_happen';
}
function get_any_db(string $name) {
    return dba_open($name, 'c', get_any_handler());
}

enum LockFlag: string {
    case FileLock = 'l';
    case DbLock = 'd';
    case NoLock = '-';
}

// TODO Test which opens a NON DB file to check for memory leaks (at least LMDB has)

function set_up_db_ex(string $handler, string $name, LockFlag $lock, bool $persistent = false) {
    $lock_flag = $lock->value;
    // Open file in creation/truncation mode
    $func = $persistent ? 'dba_popen' : 'dba_open';

    $db_file = $func($name, 'n'.$lock_flag, $handler);

    if ($db_file === false) {
        die("Failed to create DB");
    }

    // Insert some data
    dba_insert("key1", "Content String 1", $db_file);
    dba_insert("key2", "Content String 2", $db_file);
    dba_insert("key3", "Third Content String", $db_file);
    dba_insert("key4", "Another Content String", $db_file);
    dba_insert("key5", "The last content string", $db_file);

    // Insert date with array keys
    dba_insert(["", "name9"], "Content String 9", $db_file);
    dba_insert(["key10", "name10"] , "Content String 10", $db_file);
    dba_insert("[key30]name30", "Content String 30", $db_file);

    return $db_file;
}

function set_up_db(string $handler, string $name, LockFlag $lock = LockFlag::FileLock): void {
    $db_file = set_up_db_ex($handler, $name, $lock);
    // Close creation/truncation handler
    dba_close($db_file);
}

function run_common_read_only_test($dbHandle): void {
    $key = dba_firstkey($dbHandle);
    $result = [];
    while ($key) {
        $result[$key] = dba_fetch($key, $dbHandle);
        $key = dba_nextkey($dbHandle);
    }
    ksort($result);
    var_dump($result);
}

function run_standard_tests_ex(string $handler, string $name, LockFlag $lock, bool $persistent = false): void
{
    $lock_flag = $lock->value;
    set_up_db($handler, $name, $lock);
    $db_writer = dba_open($name, 'w'.$lock_flag, $handler);
    if ($db_writer === false) {
        die("Failed to open DB for write");
    }

    echo 'Remove key 1 and 3', \PHP_EOL;
    var_dump(dba_delete("key3", $db_writer));
    var_dump(dba_delete("key1", $db_writer));

    echo 'Try to remove key 1 again', \PHP_EOL;
    if ($handler !== 'lmdb') {
        var_dump(dba_delete("key1", $db_writer));
    } else {
        echo 'TODO Fix über slow LMDB on deletion of non existent key', \PHP_EOL;
    }

    // Fetch data
    $key = dba_firstkey($db_writer);
    $total_keys = 0;
    while ($key) {
        echo $key, ': ', dba_fetch($key, $db_writer), \PHP_EOL;
        $key = dba_nextkey($db_writer);
        $total_keys++;
    }
    echo 'Total keys: ', $total_keys, \PHP_EOL;
    for ($i = 1; $i < 6; $i++) {
        echo "Key $i exists? ", dba_exists("key$i", $db_writer) ? 'Y' : 'N', \PHP_EOL;
    }

    echo 'Replace second key data', \PHP_EOL;
    var_dump(dba_replace('key2', 'Content 2 replaced', $db_writer));
    echo dba_fetch('key2', $db_writer), \PHP_EOL;

    // Check that read is possible when a lock is used
    $test_flag = 't';
    if ($lock === LockFlag::NoLock) {
        // No point testing when we don't use locks
        $test_flag = '';
    }
    $db_reader = @dba_open($name, 'r'.$lock_flag.$test_flag, $handler);
    if ($db_reader === false) {
        echo 'Read during write: not allowed', \PHP_EOL;
    } else {
        echo 'Read during write: allowed', \PHP_EOL;
        dba_close($db_reader);
    }

    if (dba_insert('key number 6', 'The 6th value', $db_writer)) {
        echo 'Expected: Added a new data entry', \PHP_EOL;
    } else {
        echo 'Unexpected: Failed to add a new data entry', \PHP_EOL;
    }

    if (dba_insert('key number 6', 'The 6th value inserted again would be an error', $db_writer)) {
        echo 'Unexpected: Wrote data to already used key', \PHP_EOL;
    } else {
        echo 'Expected: Failed to insert data for already used key', \PHP_EOL;
    }

    echo 'Replace second key data', \PHP_EOL;
    var_dump(dba_replace('key2', 'Content 2 replaced 2nd time', $db_writer));
    echo 'Delete "key4"', \PHP_EOL;
    var_dump(dba_delete('key4', $db_writer));
    echo 'Fetch "key2": ', dba_fetch('key2', $db_writer), \PHP_EOL;
    echo 'Fetch "key number 6": ', dba_fetch('key number 6', $db_writer), \PHP_EOL;
    dba_close($db_writer); // when the writer is open at least db3 would fail because of buffered io.

    $db_reader = dba_open($name, 'r'.$lock_flag, $handler);
    run_common_read_only_test($db_reader);
    dba_close($db_reader);

    /* TODO popen test? Old code copied from the previous general test
    if (($db_file = dba_popen($db_filename, 'r'.($lock_flag==''?'':'-'), $handler))!==FALSE) {
        if ($handler == 'dbm' || $handler == "tcadb") {
            dba_close($db_file);
        }
    }
     */
}

function run_standard_tests(string $handler, string $name): void {
    echo '=== RUNNING WITH FILE LOCK ===', \PHP_EOL;
    ob_start();
    set_up_db($handler, $name, LockFlag::FileLock);
    run_standard_tests_ex($handler, $name, LockFlag::FileLock);
    cleanup_standard_db($name);
    $run1_output = ob_get_flush();
    echo '=== RUNNING WITH DB LOCK (default) ===', \PHP_EOL;
    ob_start();
    set_up_db($handler, $name, LockFlag::DbLock);
    run_standard_tests_ex($handler, $name, LockFlag::DbLock);
    cleanup_standard_db($name);
    $run2_output = ob_get_clean();
    if ($run1_output === $run2_output) {
        echo 'SAME OUTPUT AS PREVIOUS RUN', \PHP_EOL;
    } else {
        echo $run2_output;
    }

    echo '=== RUNNING WITH NO LOCK ===', \PHP_EOL;
    ob_start();
    set_up_db($handler, $name, LockFlag::NoLock);
    run_standard_tests_ex($handler, $name, LockFlag::NoLock);
    $run3_output = ob_get_clean();
    if ($run2_output === $run3_output) {
        echo 'SAME OUTPUT AS PREVIOUS RUN', \PHP_EOL;
    } else if ($run2_output === str_replace( // If only the fact that the lock prevented reads
            'Read during write: allowed',
            'Read during write: not allowed',
            $run3_output
        )
    ) {
        echo 'SAME OUTPUT AS PREVIOUS RUN (modulo read during write due to no lock)', \PHP_EOL;
    } else {
        echo $run3_output;
    }
}

// TODO Array keys insertion
// TODO Run all lock flags
function set_up_cdb_db_and_run(string $name): void {
    set_up_db('cdb', $name);

    $db_file = dba_open($name, 'rl', 'cdb');
    if ($db_file === false) {
        die("Failed to reopen DB");
    }
    for ($i = 1; $i < 6; $i++) {
        echo "Key $i exists? ", dba_exists("key$i", $db_file) ? 'Y' : 'N', \PHP_EOL;
    }
    run_common_read_only_test($db_file);
    dba_close($db_file);

    echo '--NO-LOCK--', \PHP_EOL;
    cleanup_standard_db($name);
    set_up_db('cdb', $name, LockFlag::NoLock);
    $db_file = dba_open($name, 'r-', 'cdb');
    if ($db_file === false) {
        die("Failed to reopen DB");
    }
    for ($i = 1; $i < 6; $i++) {
        echo "Key $i exists? ", dba_exists("key$i", $db_file) ? 'Y' : 'N', \PHP_EOL;
    }
    run_common_read_only_test($db_file);
}

function cleanup_standard_db(string $name): void {
    @unlink($name);
    @unlink($name.'.lck');
    @unlink($name.'-lock');
}
