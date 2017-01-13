--TEST--
Glob wrapper bypasses open_basedir
--INI--
open_basedir=/does_not_exist
--SKIPIF--
<?php
if (!in_array("glob", stream_get_wrappers())) echo "skip";
--FILE--
<?php

foreach ( [ __DIR__, "glob://".__DIR__ ] as $spec) {
  echo "** Opening $spec\n";
  $dir = opendir($spec);
  if (!$dir) {
    echo "Failed to open $spec\n";
    continue;
  }
  if (false === readdir($dir)) {
    echo "No files in $spec\n";
    continue;
  }
}
--EXPECTF--
** Opening %s

Warning: opendir(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (/does_not_exist) in %s%eglob-wrapper.php on line 5

Warning: opendir(%s): failed to open dir: Operation not permitted in %s%eglob-wrapper.php on line 5
Failed to open %s
** Opening glob://%s

Warning: opendir(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (/does_not_exist) in %s%eglob-wrapper.php on line 5

Warning: opendir(glob://%s): failed to open dir: operation failed in %s%eglob-wrapper.php on line 5
Failed to open glob://%s
