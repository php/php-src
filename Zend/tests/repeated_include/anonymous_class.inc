<?php

$className = AnonymousClassNameCache::get_class(fn () => new class() {
    public static function verify() {
        return 'ok';
    }
});
