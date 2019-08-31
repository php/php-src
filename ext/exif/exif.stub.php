<?php

/** @return string|false */
function exif_tagname(int $index) {}

/** @return array|false */
function exif_read_data($filename, ?string $sections_needed = null, bool $sub_arrays = false, bool $read_thumbnail = false) {}

/** @return string|false */
function exif_thumbnail($filename, &$width = null, &$height = null, &$imagetype = null) {}

/** @return int|false */
function exif_imagetype(string $filename) {}
