--TEST--
Fetching results from tables of different charsets.
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
/** This test requires MySQL Server 4.1+, which is now assumed  */
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

$link = default_mysqli_connect();

$result = mysqli_query($link, 'SHOW CHARACTER SET');
$charsets = [];
while ($row = mysqli_fetch_assoc($result)) {
    $charsets[] = $row['Charset'];
}
mysqli_free_result($result);

/* The server currently 17.07.2007 can't handle data sent in ucs2 */
/* The server currently 16.08.2010 can't handle data sent in utf16 and utf32 */
/* The server currently 02.09.2011 can't handle data sent in utf16le */
/* As of MySQL 8.0.28, `SHOW CHARACTER SET` contains utf8mb3, but that is not yet supported by mysqlnd */
const UNSUPPORTED_ENCODINGS = [
    'ucs2',
    'utf16',
    'utf32',
    'utf16le',
    'utf8mb3',
];

foreach ($charsets as $charset) {
    if (in_array($charset, UNSUPPORTED_ENCODINGS, true)) {
        continue;
    }

    $sql = sprintf("CREATE TABLE test_mysqli_set_charset(id INT, label CHAR(1)) CHARACTER SET '%s' ", $charset);
    mysqli_query($link, $sql);

    mysqli_set_charset($link, $charset);

    for ($i = 1; $i <= 3; $i++) {
        mysqli_query(
            $link,
            sprintf(
                "INSERT INTO test_mysqli_set_charset (id, label) VALUES (%d, '%s')",
                $i,
                mysqli_real_escape_string($link, chr(ord("a") + $i))
            )
        );
    }

    $result = mysqli_query($link, "SELECT id, label FROM test_mysqli_set_charset");
    for ($i = 1; $i <= 3; $i++) {
        $tmp = mysqli_fetch_assoc($result);
        if ($tmp['id'] != $i)
            printf("[012 + %s] Expecting %d, got %s, [%d] %s\n", $charset,
                    $i, $tmp['id'],
                    mysqli_errno($link), mysqli_error($link));

        if ($tmp['label'] != chr(ord("a") + $i))
            printf("[013 + %s] Expecting %d, got %s, [%d] %s\n", $charset,
                chr(ord("a") + $i), $tmp['label'],
                mysqli_errno($link), mysqli_error($link));
    }
    mysqli_free_result($result);
    mysqli_query($link, "DROP TABLE test_mysqli_set_charset");
}

mysqli_close($link);

print "done!";
?>
--CLEAN--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
tear_down_table_on_default_connection('test_mysqli_set_charset');
?>
--EXPECT--
done!
