--TEST--
mysqli_stmt_attr_set() - mysqlnd does not check for invalid codes
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
    require 'table.inc';

    $stmt = mysqli_stmt_init($link);
    try {
        mysqli_stmt_attr_set($stmt, 0, 0);
    } catch (\Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . PHP_EOL;
    }

    $stmt->prepare("SELECT * FROM test");
    // Invalid Attribute (2nd argument)
    try {
        mysqli_stmt_attr_set($stmt, -1, 0);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    // Invalid mode for MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH
    try {
        $stmt->attr_set(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, -1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    $stmt->close();

    //
    // MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH
    //
    // expecting max_length not to be set and be 0 in all cases
    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT label FROM test");
    $stmt->execute();
    $stmt->store_result();
    $res = $stmt->result_metadata();
    $fields = $res->fetch_fields();
    foreach ($fields as $meta) {
        if ($meta->max_length !== 0)
            printf("[007] max_length should be not set (= 0), got %s for field %s\n", $meta->max_length, $meta->name);
    }
    $res->close();
    $stmt->close();

    // MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH is no longer supported, expect no change in behavior.
    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT label FROM test");
    var_dump($stmt->attr_set(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, 1));
    $res = $stmt->attr_get(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH);
    if ($res !== 1)
        printf("[007.1] max_length should be 1, got %s\n", $res);
    $stmt->execute();
    $stmt->store_result();
    $res = $stmt->result_metadata();
    $fields = $res->fetch_fields();
    foreach ($fields as $meta) {
        if ($meta->max_length !== 0)
            printf("[008] max_length should be not set (= 0), got %s for field %s\n", $meta->max_length, $meta->name);
    }
    $res->close();
    $stmt->close();

    // expecting max_length not to be set
    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT label FROM test");
    $stmt->attr_set(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, 0);
    $res = $stmt->attr_get(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH);
    if ($res !== 0)
        printf("[008.1] max_length should be 0, got %s\n", $res);
    $stmt->execute();
    $stmt->store_result();
    $res = $stmt->result_metadata();
    $fields = $res->fetch_fields();
    foreach ($fields as $meta) {
        if ($meta->max_length !== 0)
            printf("[009] max_length should not be set (= 0), got %s for field %s\n", $meta->max_length, $meta->name);
    }
    $res->close();
    $stmt->close();

    //
    // Cursors
    //


    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT id, label FROM test");

    // Invalid cursor type
    try {
        $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, -1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_NO_CURSOR)))
        printf("[013] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY)))
        printf("[014] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

    $stmt->close();

    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT id, label FROM test");
    $stmt->execute();
    $id = $label = NULL;
    $stmt->bind_result($id, $label);
    $results = array();
    while ($stmt->fetch())
        $results[$id] = $label;
    $stmt->close();
    if (empty($results))
        printf("[015] Results should not be empty, subsequent tests will probably fail!\n");

    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT id, label FROM test");
    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_NO_CURSOR)))
        printf("[016] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
    $stmt->execute();
    $id = $label = NULL;
    $stmt->bind_result($id, $label);
    $results2 = array();
    while ($stmt->fetch())
        $results2[$id] = $label;
    $stmt->close();
    if ($results != $results2) {
        printf("[017] Results should not differ. Dumping both result sets.\n");
        var_dump($results);
        var_dump($results2);
    }

    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT id, label FROM test");
    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY)))
        printf("[018] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
    $stmt->execute();
    $id = $label = NULL;
    $stmt->bind_result($id, $label);
    $results2 = array();
    while ($stmt->fetch())
        $results2[$id] = $label;
    $stmt->close();
    if ($results != $results2) {
        printf("[019] Results should not differ. Dumping both result sets.\n");
        var_dump($results);
        var_dump($results2);
    }

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
Error: mysqli_stmt object is not fully initialized
mysqli_stmt_attr_set(): Argument #2 ($attribute) must be either MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH or MYSQLI_STMT_ATTR_CURSOR_TYPE
mysqli_stmt::attr_set(): Argument #2 ($value) must be 0 or 1 for attribute MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH
bool(true)
mysqli_stmt::attr_set(): Argument #2 ($value) must be one of the MYSQLI_CURSOR_TYPE_* constants for attribute MYSQLI_STMT_ATTR_CURSOR_TYPE
done!
