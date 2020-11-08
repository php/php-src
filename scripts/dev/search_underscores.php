#!/usr/bin/env php
<?php

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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* This script lists extension-, class- and method names that contain any
   underscores. It omits magic names (e.g. anything that starts with two
   underscores but no more).
 */

$cnt_modules = 0;
$cnt_classes = 0;
$cnt_methods = 0;
$err = 0;

$classes = array_merge(get_declared_classes(), get_declared_interfaces());

$extensions = array();

foreach(get_loaded_extensions() as $ext) {
    $cnt_modules++;
    if (strpos($ext, "_") !== false) {
        $err++;
        $extensions[$ext] = array();
    }
}

$cnt_classes = count($classes);

foreach($classes as $c) {
    if (strpos($c, "_") !== false) {
        $err++;
        $ref = new ReflectionClass($c);
        if (!($ext = $ref->getExtensionName())) {;
            $ext = $ref->isInternal() ? "<internal>" : "<user>";
        }
        if (!array_key_exists($ext, $extensions)) {
            $extensions[$ext] = array();
        }
        $extensions[$ext][$c] = array();
        foreach(get_class_methods($c) as $method) {
            $cnt_methods++;
            if (strpos(substr($method, substr($method, 0, 2) != "__"  ? 0 : 2), "_") !== false) {
                $err++;
                $extensions[$ext][$c][] = $method;
            }
        }
    }
    else
    {
        $cnt_methods += count(get_class_methods($c));
    }
}

$cnt = $cnt_modules + $cnt_classes + $cnt_methods;

printf("\n");
printf("Modules: %5d\n", $cnt_modules);
printf("Classes: %5d\n", $cnt_classes);
printf("Methods: %5d\n", $cnt_methods);
printf("\n");
printf("Names:   %5d\n", $cnt);
printf("Errors:  %5d (%.1f%%)\n", $err, round($err * 100 / $cnt, 1));
printf("\n");

ksort($extensions);
foreach($extensions as $ext => &$classes) {
    echo "Extension: $ext\n";
    ksort($classes);
    foreach($classes as $classname => &$methods) {
        echo "  Class: $classname\n";
        ksort($methods);
        foreach($methods as $method) {
            echo "    Method: $method\n";
        }
    }
}

printf("\n");

?>
