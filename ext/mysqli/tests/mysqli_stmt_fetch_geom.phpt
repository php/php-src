--TEST--
mysqli_stmt_fetch - geometry / spatial types
--SKIPIF--
<?php
    require_once('skipif.inc');
    require_once('skipifemb.inc');
    require_once('skipifconnectfailure.inc');

    if (!defined("MYSQLI_TYPE_GEOMETRY"))
        die("skip MYSQLI_TYPE_GEOMETRY not defined");
?>
--FILE--
<?php
    require('connect.inc');
    if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

    function func_mysqli_stmt_fetch_geom($link, $engine, $sql_type, $bind_value, $offset) {

        if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
            printf("[%04d] [%d] %s\n", $offset, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if (!mysqli_query($link, sprintf("CREATE TABLE test(id INT, label %s, PRIMARY KEY(id)) ENGINE = %s", $sql_type, $engine))) {
            // don't bail - column type might not be supported by the server, ignore this
            return false;
        }

        for ($id = 1; $id < 4; $id++) {
            $sql = sprintf("INSERT INTO test(id, label) VALUES (%d, %s)", $id, $bind_value);
            if (!mysqli_query($link, $sql)) {
                printf("[%04d] [%d] %s\n", $offset + 2 + $id, mysqli_errno($link), mysqli_error($link));
            }
        }

        if (!$stmt = mysqli_stmt_init($link)) {
            printf("[%04d] [%d] %s\n", $offset + 6, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if (!mysqli_stmt_prepare($stmt, "SELECT id, label FROM test")) {
            printf("[%04d] [%d] %s\n", $offset + 7, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            mysqli_stmt_close($stmt);
            return false;
        }

        if (!mysqli_stmt_execute($stmt) || !mysqli_stmt_store_result($stmt)) {
            printf("[%04d] [%d] %s\n", $offset + 8, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            mysqli_stmt_close($stmt);
            return false;
        }

        if (!mysqli_stmt_bind_result($stmt, $id, $bind_res)) {
            printf("[%04d] [%d] %s\n", $offset + 9, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
            mysqli_stmt_close($stmt);
            return false;
        }

        $result = mysqli_stmt_result_metadata($stmt);
        $fields = mysqli_fetch_fields($result);
        if ($fields[1]->type != MYSQLI_TYPE_GEOMETRY) {
            printf("[%04d] [%d] %s wrong type %d\n", $offset + 10, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt), $fields[1]->type);
        }

        $num = 0;
        $rows = array();
        while (true === @mysqli_stmt_fetch($stmt)) {
            $rows[] = array('id' => $id, 'label' => $bind_res);
            $num++;
        }

        if ($num != 3) {
            printf("[%04d] [%d] %s, expecting 3 results, got only %d results\n",
                $offset + 17, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt), $num);
            return false;
        }
        mysqli_stmt_close($stmt);

        foreach ($rows as $row) {
            if (!$stmt = mysqli_stmt_init($link)) {
                printf("[%04d] [%d] %s\n", $offset + 10, mysqli_errno($link), mysqli_error($link));
                return false;
            }

            if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)")) {
                printf("[%04d] [%d] %s\n", $offset + 11, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }

            $new_id = $row['id'] + 10;
            if (!mysqli_stmt_bind_param($stmt, "is", $new_id, $row['label'])) {
                printf("[%04d] [%d] %s\n", $offset + 12, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }

            if (!mysqli_stmt_execute($stmt)) {
                printf("[%04d] [%d] %s\n", $offset + 13, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }
            mysqli_stmt_close($stmt);

            if (!$res_normal = mysqli_query($link, sprintf("SELECT id, label FROM test WHERE id = %d",
                    $new_id))) {
                printf("[%04d] [%d] %s\n", $offset + 14, mysqli_errno($link), mysqli_error($link));
                return false;
            }

            if (!$row_normal = mysqli_fetch_assoc($res_normal)) {
                printf("[%04d] [%d] %s\n", $offset + 15, mysqli_errno($link), mysqli_error($link));
                return false;
            }

            if ($row_normal['label'] != $row['label']) {
                printf("[%04d] PS and non-PS return different data.\n", $offset + 16);
                return false;
            }
            mysqli_free_result($res_normal);
        }

        return true;
    }

    $geomFromText = $link->server_version >= 80000 ? "ST_GeomFromText" : "GeomFromText";
    func_mysqli_stmt_fetch_geom($link, $engine, "GEOMETRY", "$geomFromText('POINT(2 2)')", 20);
    func_mysqli_stmt_fetch_geom($link, $engine, "POINT", "$geomFromText('POINT(1 1)')", 40);
    func_mysqli_stmt_fetch_geom($link, $engine, "LINESTRING", "$geomFromText('LINESTRING(0 0,1 1,2 2)')", 60);
    func_mysqli_stmt_fetch_geom($link, $engine, "POLYGON", "$geomFromText('POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))')", 80);
    func_mysqli_stmt_fetch_geom($link, $engine, "MULTIPOINT", "$geomFromText('MULTIPOINT(1 1, 2 2)')", 100);
    func_mysqli_stmt_fetch_geom($link, $engine, "MULTILINESTRING", "$geomFromText('MULTILINESTRING((0 0,1 1,2 2),(0 0,1 1,3 3))')", 120);
    func_mysqli_stmt_fetch_geom($link, $engine, "MULTIPOLYGON", "$geomFromText('MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5)),((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5)))')", 140);
    func_mysqli_stmt_fetch_geom($link, $engine, "GEOMETRYCOLLECTION", "$geomFromText('GEOMETRYCOLLECTION(POINT(1 1),LINESTRING(0 0,1 1,2 2,3 3,4 4))')", 160);

    mysqli_close($link);
    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
