--TEST--
mysqli_stmt_bind_param() - playing with references
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require('table.inc');

    function findRow($offset, $link, $id, $label) {

        $sql = sprintf("SELECT id, label FROM test WHERE id = '%d' AND label = '%s'",
                $id, $label);
        if (!$res = mysqli_query($link, $sql)) {
            printf("[%03d + 1] %s failed, [%d] %s\n",
                $offset, $sql, mysqli_errno($link), mysqli_error($link));
            return false;
        }
        if (!$row = mysqli_fetch_assoc($res)) {
            printf("[%03d + 2] fetch for %s failed, [%d] %s\n",
                $offset, $sql, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        mysqli_free_result($res);
        if ($row['id'] != $id) {
            printf("[%03d + 3] Expecting %s/%s got %s/%s\n",
                $offset, gettype($id), $id,
                gettype($row['id']), $row['id']
                );
            return false;
        }

        if ($row['label'] != $label) {
            printf("[%03d + 4] Expecting %s/%s got %s/%s\n",
                $offset, gettype($label), $label,
                gettype($row['label']), $row['label']
                );
            return false;
        }

        $sql = sprintf("DELETE FROM test WHERE id = '%d' AND label = '%s'",
                $id, $label);
        if (!mysqli_query($link, $sql)) {
            printf("[%03d + 5] %s failed, [%d] %s\n",
                $offset, $sql, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        return true;
    }
    // or we will get dups around [28]
    mysqli_query($link, "ALTER TABLE test DROP PRIMARY KEY");

    $stmt = mysqli_stmt_init($link);
    if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
        printf("[001] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

    $id = 100;
    $label = 'v';
    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
        printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[003] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    // no need to check the return value, will bail and make EXPECTF fail if need be
    findRow(4, $link, $id, $label);

    $id++;
    $label_ref = &$label;
    $label = 'w';
    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label_ref)))
        printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[006] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(7, $link, $id, $label_ref);

    $id++;
    $label_ref_ref = &$label_ref;
    $label = 'x';

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label_ref_ref)))
        printf("[007] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(9, $link, $id, $label_ref_ref);

    $id = 9;
    $label = $id;
    $label_num = &$label;

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label_num)))
        printf("[010] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[011] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(12, $link, $id, $label_num);

    $label_num = &$id;
    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label_num)))
        printf("[013] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[014] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(15, $link, $id, $label_num);

    $label = 9;
    $id = &$label;

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
        printf("[015] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(17, $link, $id, $label);

    $base = 9;
    $id = &$base;
    $label = &$id;

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
        printf("[018] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[019] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(20, $link, $id, $label);

    $id_ref = &$id;
    $label_ref = &$label;

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id_ref, $label_ref)))
        printf("[021] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[022] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(23, $link, $id_ref, $label_ref);

    $id_ref_ref = &$GLOBALS['id_ref'];
    $label_ref_ref = &$GLOBALS['label_ref_ref'];

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id_ref_ref, $label_ref_ref)))
        printf("[024] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[025] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(26, $link, $id_ref_ref, $label_ref_ref);

    unset($id);
    unset($label);
    $id = 102;
    $label = new stdClass();
    $label->label = 'y';
    $id_ref = &$GLOBALS['id'];
    $label_ref = &$label->label;
    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id_ref, $label_ref)))
        printf("[027] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
    if (true !== @mysqli_stmt_execute($stmt))
        printf("[028] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(29, $link, $id_ref, $label_ref);

    $id = 103;
    $label_a = &$label_b;
    $label_b = &$label_a;
    $label_a = 'z';

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label_b)))
        printf("[030] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[031] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(32, $link, $id, $label_b);

    class foo {
        public $foo;
        function __construct() {
            $this->foo = &$this->bar;
        }
    }
    class bar extends foo {
        public $bar = 'v';
    }
    $bar = new bar();
    $id++;
    $label = &$GLOBALS['bar']->foo;

    if (true !== ($tmp = mysqli_stmt_bind_param($stmt, "is", $id, $label)))
        printf("[033] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    if (true !== mysqli_stmt_execute($stmt))
        printf("[034] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
    findRow(35, $link, $id, $label);

    mysqli_stmt_close($stmt);
    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
