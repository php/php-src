<?php

function nb_is_readable($stream, $timeout = 1) {
    $r = [$stream]; $w = []; $e = [];
    return (bool) stream_select($r, $w, $e, $timeout, 0);
};
function nb_is_writable($stream, $timeout = 1) {
    $r = []; $w = [$stream]; $e = [];
    return (bool) stream_select($r, $w, $e, $timeout, 0);
};
function nb_flush($db, $db_socket) {
    while (TRUE) {
        if (! nb_is_writable($db_socket)) {
            continue;
        }
        $flush = pg_flush($db);
        if ($flush === TRUE) {
            break; // All data flushed
        } elseif ($flush === FALSE) {
            echo "pg_flush() error\n";
            break;
        }
    }
};
function nb_consume($db, $db_socket) {
    while (TRUE) {
        if (!nb_is_readable($db_socket)) {
            continue;
        } elseif (!pg_consume_input($db)) {
            echo "pg_consume_input() error\n";
            break;
        } elseif (!pg_connection_busy($db)) {
            break; // All data consumed
        }

    }
};

