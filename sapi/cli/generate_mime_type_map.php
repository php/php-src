#!/usr/bin/env php
<?php

// Check if we are being given a mime.types file or if we should use the
// default URL.
$source = count($argv) > 1 ? $argv[1] : 'https://cdn.jsdelivr.net/gh/jshttp/mime-db@v1.45.0/db.json';

// See if we can actually load it.
$data = @file_get_contents($source);
if ($data === false) {
    fprintf(STDERR, "Error: unable to read $source\n");
    exit(1);
}

// Source preference from lowest to highest
$prefs = ['nginx' => 1, 'apache' => 2, 'custom' => 3, 'iana' => 4];

$extensions = [];
$types = json_decode($data, true);
foreach ($types as $mime => $info) {
    $source = $info['source'] ?? 'custom';
    $pref = $prefs[$source];
    foreach ($info['extensions'] ?? [] as $extension) {
        if (isset($extensions[$extension])) {
            // Use same preference rules as jshttp/mime-types
            [$oldMime, $oldPref] = $extensions[$extension];
            if ($oldMime !== 'application/octet-stream'
                && ($oldPref > $pref
                    || ($oldPref === $pref && substr($oldMime, 0, 12) === 'application/'))) {
                continue;
            }
        }

        $extensions[$extension] = [$mime, $pref];
    }
}

// Only keep the MIME type.
$extensions = array_map(function($x) { return $x[0]; }, $extensions);

$additional_mime_maps = [
    "jsm" => "application/javascript",
];

foreach($additional_mime_maps as $ext => $mime) {
    if (!isset($extensions[$ext])) {
        $extensions[$ext] = $mime;
    } else {
        fprintf(STDERR, "Ignored exist mime type: $ext => $mime\n");
    }
}

uksort($extensions, function($ext1, $ext2) {
    return strcmp($ext1, $ext2);
});

echo <<<HEADER
/*
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
\tconst char *ext;
\tconst char *mime_type;
} php_cli_server_ext_mime_type_pair;

static const php_cli_server_ext_mime_type_pair mime_type_map[] = {

HEADER;

foreach ($extensions as $extension => $mime) {
    echo "\t{ \"" .addcslashes($extension, "\0..\37!@\@\177..\377") . "\", \""
        . addcslashes($mime, "\0..\37!@\@\177..\377") . "\" },\n";
}

echo <<<FOOTER
\t{ NULL, NULL }
};

#endif /* PHP_CLI_SERVER_MIME_TYPE_MAP_H */

FOOTER;
