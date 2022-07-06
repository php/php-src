<?php

/** @generate-function-entries */

function exif_tagname(int $index): string|false {}

/** @param resource|string $file */
function exif_read_data($file, ?string $required_sections = null, bool $as_arrays = false, bool $read_thumbnail = false): array|false {}

/**
 * @param resource|string $file
 * @param int $width
 * @param int $height
 * @param int $image_type
 */
function exif_thumbnail($file, &$width = null, &$height = null, &$image_type = null): string|false {}

function exif_imagetype(string $filename): int|false {}
