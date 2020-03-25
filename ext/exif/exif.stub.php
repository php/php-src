<?php

function exif_tagname(int $index): string|false {}

function exif_read_data($filename, ?string $sections_needed = null, bool $sub_arrays = false, bool $read_thumbnail = false): array|false {}

function exif_thumbnail($filename, &$width = null, &$height = null, &$imagetype = null): string|false {}

function exif_imagetype(string $filename): int|false {}
