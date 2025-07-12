<?php

function removeDirRecursive($dir) {
    if (!is_dir($dir)) return;
    try {
        $iterator = new RecursiveIteratorIterator(
            new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
            RecursiveIteratorIterator::CHILD_FIRST
        );
        foreach ($iterator as $fileinfo) {
            if ($fileinfo->isDir()) {
                @rmdir($fileinfo->getRealPath());
            } else {
                @unlink($fileinfo->getRealPath());
            }
        }
        @rmdir($dir);
    } catch (UnexpectedValueException $e) { @rmdir($dir); } catch (Exception $e) { @rmdir($dir); }
}