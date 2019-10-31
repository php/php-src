<?php

final class WeakReference {
    public function __construct();

    public static function create(): WeakReference;

    public function get(): ?object;
}
