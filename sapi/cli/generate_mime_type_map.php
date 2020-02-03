#!/usr/bin/env php
<?php

// Check if we are being given a mime.types file or if we should use the
// default URL.
$source = count($_SERVER['argv']) > 1 ? $_SERVER['argv'][1] : 'https://raw.githubusercontent.com/apache/httpd/trunk/docs/conf/mime.types';

// See if we can actually load it.
$types = @file($source);
if ($types === false) {
    fprintf(STDERR, "Error: unable to read $source\n");
    exit(1);
}

// Remove comments and flip into an extensions array.
$extensions = [];
array_walk($types, function ($line) use (&$extensions) {
    $line = trim($line);
    if ($line && $line[0] != '#') {
        $fields = preg_split('/\s+/', $line);
        if (count($fields) > 1) {
            $mime = array_shift($fields);
            foreach ($fields as $extension) {
                $extensions[$extension] = $mime;
            }
        }
    }
});

$additional_mime_maps = [
    "map" => "application/json",	// from commit: a0d62f08ae8cbebc88e5c92e08fca8d0cdc7309d
    "jsm" => "application/javascript",
];

foreach($additional_mime_maps as $ext => $mime) {
    if (!isset($extensions[$ext])) {
        $extensions[$ext] = $mime;
    } else {
        printf(STDERR, "Ignored exist mime type: $ext => $mime\n");
    }
}

?>
/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
*/

/* This is a generated file. Rather than modifying it, please run
 * "php generate_mime_type_map.php > mime_type_map.h" to regenerate the file. */

#ifndef PHP_CLI_SERVER_MIME_TYPE_MAP_H
#define PHP_CLI_SERVER_MIME_TYPE_MAP_H

typedef struct php_cli_server_ext_mime_type_pair {
    const char *ext;
    const char *mime_type;
} php_cli_server_ext_mime_type_pair;

static const php_cli_server_ext_mime_type_pair mime_type_map[] = {
<?php foreach ($extensions as $extension => $mime): ?>
    { "<?= addcslashes($extension, "\0..\37!@\@\177..\377") ?>", "<?= addcslashes($mime, "\0..\37!@\@\177..\377") ?>" },
<?php endforeach ?>
    { NULL, NULL }
};

#endif /* PHP_CLI_SERVER_MIME_TYPE_MAP_H */
