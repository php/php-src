<?php
enum E: string {
    case A = 'a';
}

enum F: string {
    case B = 'b';
}

trait T {
    protected const E X = E::A;
    public const E|F Y = F::B;
}

class C {
    use T;

    public static function x() {
        return self::X;
    }
}
