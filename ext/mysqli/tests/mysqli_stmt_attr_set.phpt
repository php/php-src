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

    $valid_attr = array(MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH);
    $valid_attr[] = MYSQLI_STMT_ATTR_CURSOR_TYPE;
    $valid_attr[] =	MYSQLI_CURSOR_TYPE_NO_CURSOR;
    $valid_attr[] =	MYSQLI_CURSOR_TYPE_READ_ONLY;
    $valid_attr[] =	MYSQLI_CURSOR_TYPE_FOR_UPDATE;
    $valid_attr[] =	MYSQLI_CURSOR_TYPE_SCROLLABLE;
    $valid_attr[] = MYSQLI_STMT_ATTR_PREFETCH_ROWS;


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
    $max_lengths = array();
    foreach ($fields as $k => $meta) {
        $max_lengths[$meta->name] = $meta->max_length;
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
    $max_lengths = array();
    foreach ($fields as $k => $meta) {
        $max_lengths[$meta->name] = $meta->max_length;
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
    $max_lengths = array();
    foreach ($fields as $k => $meta) {
        $max_lengths[$meta->name] = $meta->max_length;
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

    if (false !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_FOR_UPDATE)))
        printf("[011] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (false !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_SCROLLABLE)))
        printf("[012] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

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


    //
    // MYSQLI_STMT_ATTR_PREFETCH_ROWS
    //

    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT id, label FROM test");
    // Invalid prefetch value
    try {
        $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, 0);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, 1)))
        printf("[020] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
    $stmt->execute();
    $id = $label = NULL;
    $stmt->bind_result($id, $label);
    $results = array();
    while ($stmt->fetch())
        $results[$id] = $label;
    $stmt->close();
    if (empty($results))
        printf("[021] Results should not be empty, subsequent tests will probably fail!\n");

    /* prefetch is not supported
    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT label FROM test");
    if (false !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, -1)))
        printf("[022] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
    $stmt->close();

    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT label FROM test");
    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, PHP_INT_MAX)))
            printf("[023] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
    $stmt->close();

    $stmt = mysqli_stmt_init($link);
    $stmt->prepare("SELECT id, label FROM test");
    if (true !== ($tmp = $stmt->attr_set(MYSQLI_STMT_ATTR_PREFETCH_ROWS, 2)))
        printf("[024] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);
    $stmt->execute();
    $id = $label = NULL;
    $stmt->bind_result($id, $label);
    $results2 = array();
    while ($stmt->fetch())
        $results2[$id] = $label;
    $stmt->close();
    if ($results != $results2) {
        printf("[025] Results should not differ. Dumping both result sets.\n");
        var_dump($results);
        var_dump($results2);
    }
    */

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
Error: mysqli_stmt object is not fully initialized
mysqli_stmt_attr_set(): Argument #2 ($attribute) must be one of MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH, MYSQLI_STMT_ATTR_PREFETCH_ROWS, or STMT_ATTR_CURSOR_TYPE
mysqli_stmt::attr_set(): Argument #2 ($value) must be 0 or 1 for attribute MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH
bool(true)
mysqli_stmt::attr_set(): Argument #2 ($value) must be one of the MYSQLI_CURSOR_TYPE_* constants for attribute MYSQLI_STMT_ATTR_CURSOR_TYPE
mysqli_stmt::attr_set(): Argument #2 ($value) must be greater than 0 for attribute MYSQLI_STMT_ATTR_PREFETCH_ROWS
done!
