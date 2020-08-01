<?php

/** @generate-function-entries */

function exif_tagname(int $index): string|false {}

/** @param resource|string $filename */
function exif_read_data($filename, ?string $sections_needed = null, bool $sub_arrays = false, bool $read_thumbnail = false): array|false {}

/** @param resource|string $filename */
function exif_thumbnail($filename, &$width = null, &$height = null, &$imagetype = null): string|false {}

function exif_imagetype(string $filename): int|false {}
