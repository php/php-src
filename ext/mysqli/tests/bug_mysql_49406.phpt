--TEST--
MySQL Bug #49406 (Binding params doesn't work when selecting a date inside a CASE-WHEN, http://bugs.mysql.com/bug.php?id=49406)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    if (!($link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    $query = "SELECT CASE  WHEN 0 THEN CAST('2009-12-03' AS DATE)  ELSE CAST('2009-12-03' AS DATE) END";

    if (!$res = $link->query($query))
        printf("[002] [%d] %s\n", $link->errno, $link->error);

    if (!$row = $res->fetch_row())
        printf("[003] No result, [%d] %s\n", $link->errno, $link->error);

    $res->free();

    if ($row[0] != '2009-12-03') {
        printf("[004] Expecting '2009-12-03' got '%s'\n", $row[0]);
    }

    if (!$stmt = $link->prepare($query))
        printf("[005] [%d] %s\n", $link->errno, $link->error);

    if (!$stmt->execute() || !$stmt->store_result())
        printf("[006] [%d] %s\n", $stmt->errno, $stmt->error);


    $datatypes = array(
        MYSQLI_TYPE_TINY => "TINY",
        MYSQLI_TYPE_SHORT => "SHORT",
        MYSQLI_TYPE_LONG => "LONG",
        MYSQLI_TYPE_FLOAT => "FLOAT",
        MYSQLI_TYPE_DOUBLE => "DOUBLE",
        MYSQLI_TYPE_TIMESTAMP => "TIMESTAMP",
        MYSQLI_TYPE_LONGLONG => "LONGLONG",
        MYSQLI_TYPE_INT24 => "INT24",
        MYSQLI_TYPE_DATE => "DATE",
        MYSQLI_TYPE_TIME => "TIME",
        MYSQLI_TYPE_DATETIME => "DATETIME",
        MYSQLI_TYPE_YEAR => "YEAR",
        MYSQLI_TYPE_ENUM => "ENUM",
        MYSQLI_TYPE_SET	=> "SET",
        MYSQLI_TYPE_TINY_BLOB => "TINYBLOB",
        MYSQLI_TYPE_MEDIUM_BLOB => "MEDIUMBLOB",
        MYSQLI_TYPE_LONG_BLOB => "LONGBLOB",
        MYSQLI_TYPE_BLOB => "BLOB",
        MYSQLI_TYPE_VAR_STRING => "VAR_STRING",
        MYSQLI_TYPE_STRING => "STRING",
        MYSQLI_TYPE_NULL => "NULL",
        MYSQLI_TYPE_NEWDATE => "NEWDATE",
        MYSQLI_TYPE_INTERVAL => "INTERVAL",
        MYSQLI_TYPE_GEOMETRY => "GEOMETRY",
    );

    $meta_res = $stmt->result_metadata();
    for ($field_idx = 0; $field_idx < $meta_res->field_count; $field_idx++) {
        $field = $meta_res->fetch_field();
        printf("Field        : %d\n", $field_idx);
        printf("Name         : %s\n", $field->name);
        printf("Orgname      : %s\n", $field->orgname);
        printf("Table        : %s\n", $field->table);
        printf("Orgtable     : %s\n", $field->orgtable);
        printf("Maxlength    : %d\n", $field->max_length);
        printf("Length       : %d\n", $field->length);
        printf("Charsetnr    : %d\n", $field->charsetnr);
        printf("Flags        : %d\n", $field->flags);
        printf("Type         : %d (%s)\n", $field->type, (isset($datatypes[$field->type])) ? $datatypes[$field->type] : 'unknown');
        printf("Decimals     : %d\n", $field->decimals);
    }

    $row_stmt = null;
    if (!$stmt->bind_result($row_stmt) || !$stmt->fetch())
        printf("[007] [%d] %s\n", $stmt->errno, $stmt->error);

    if ($row[0] != $row_stmt) {
        printf("[008] PS and non-PS results differ, dumping data\n");
        var_dump($row[0]);
        var_dump($row_stmt);
    }

    $stmt->close();
    $link->close();

    echo "done";
?>
--EXPECTF--
Field        : %s
Name         : %s
Orgname      :%s
Table        :%s
Orgtable     :%s
Maxlength    : %d
Length       : %d
Charsetnr    : %d
Flags        : %d
Type         : %d (%s)
Decimals     : %d
done
