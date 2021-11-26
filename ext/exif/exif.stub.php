<?php

/** @generate-class-entries */

/** @refcount 1 */
function exif_tagname(int $index): string|false {}

/**
 * @param resource|string $file
 * @return array<string, mixed>|false
 * @refcount 1
 */
function exif_read_data($file, ?string $required_sections = null, bool $as_arrays = false, bool $read_thumbnail = false): array|false {}

/**
 * @param resource|string $file
 * @param int $width
 * @param int $height
 * @param int $image_type
 * @refcount 1
 */
function exif_thumbnail($file, &$width = null, &$height = null, &$image_type = null): string|false {}

function exif_imagetype(string $filename): int|false {}
