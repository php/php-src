--TEST--
Examples of the usage of gmp for elliptic curve cryptography.
--DESCRIPTION--
DANGER: DO NOT USE IN SECURITY-RELATED USE-CASES.
This implementation is not hardened or tested against side channels (e.g. time or cache).
Side-channels as contained in this implementation may compromise secrets (e.g. secret keys).
Hence, it MUST NOT BE USED IN SECURITY-RELATED USE-CASES.

This implementation operates on the secp256r1 curve from https://www.secg.org/sec2-v2.pdf (also known as NIST P-256).
For addition and doublication, it implements https://www.secg.org/sec1-v2.pdf (2.2.1).
For point decompression, it implements https://www.secg.org/sec1-v2.pdf (2.3.4).
For scalar multiplication, it uses the well-known double-add-always pardigm.

The implementation executes a diffie-hellman handshake.
Omitted is an explicit demonstration of (public-key) encryption, commitments, zero-knowledge proofs or similar common applications.
However, the operations used for diffie-hellman is at the core of all these other applications, hence these use-cases are implicitly covered.

$aliceSecret and $bobSecret generated with
$random = gmp_random_range(0, $n);
$randomHex = strtoupper(gmp_strval($random, 16));
echo chunk_split($randomHex, 8, " ");
--EXTENSIONS--
gmp
--FILE--
<?php

/**
 * Elliptic curve point with x and y coordinates
 */
class Point
{
    public function __construct(public \GMP $x, public \GMP $y)
    {
    }

    public static function createInfinity(): Point
    {
        return new Point(gmp_init(0), gmp_init(0));
    }

    public function isInfinity(): bool
    {
        return gmp_cmp($this->x, 0) === 0 && gmp_cmp($this->y, 0) === 0;
    }

    public function equals(self $other): bool
    {
        return gmp_cmp($this->x, $other->x) === 0 && gmp_cmp($this->y, $other->y) === 0;
    }
}


/**
 * In the finite field F_p,
 * an elliptic curve in the short Weierstrass form y^2 = x^3 + ax + b is defined,
 * forming a group over addition.
 *
 * A base point G of order n and cofactor h is picked in this group.
 */
class Curve
{
    public function __construct(
        private readonly \GMP $p,
        private readonly \GMP $a,
        private readonly \GMP $b,
        private readonly Point $G,
        private readonly \GMP $n
    ) {}

    public function getP(): \GMP
    {
        return $this->p;
    }

    public function getA(): \GMP
    {
        return $this->a;
    }

    public function getB(): \GMP
    {
        return $this->b;
    }

    public function getG(): Point
    {
        return $this->G;
    }

    public function getN(): \GMP
    {
        return $this->n;
    }
}


/**
 * Math inside a prime field; hence always (mod p)
 */
class PrimeField
{
    private int $elementBitLength;

    public function __construct(private readonly \GMP $prime)
    {
        $this->elementBitLength = strlen(gmp_strval($prime, 2));
    }

    public function getElementBitLength(): int
    {
        return $this->elementBitLength;
    }

    public function add(\GMP $a, \GMP $b): \GMP
    {
        $r = gmp_add($a, $b);
        return gmp_mod($r, $this->prime);
    }

    public function mul(\GMP $a, \GMP $b): \GMP
    {
        $r = gmp_mul($a, $b);
        return gmp_mod($r, $this->prime);
    }

    public function sub(\GMP $a, \GMP $b): \GMP
    {
        $r = gmp_sub($a, $b);
        return gmp_mod($r, $this->prime);
    }

    public function mod(\GMP $a): \GMP
    {
        return gmp_mod($a, $this->prime);
    }

    public function invert(\GMP $z): \GMP|false
    {
        return gmp_invert($z, $this->prime);
    }
}

class UnsafePrimeCurveMath
{
    private PrimeField $field;
    public function __construct(private readonly Curve $curve)
    {
        $this->field = new PrimeField($this->curve->getP());
    }

    /**
    * checks whether point fulfills the defining equation of the curve
    */
    public function isOnCurve(Point $point): bool
    {
        $left = gmp_pow($point->y, 2);
        $right = gmp_add(
            gmp_add(
                gmp_pow($point->x, 3),
                gmp_mul($this->curve->getA(), $point->x)
            ),
            $this->curve->getB()
        );

        $comparison = $this->field->sub($left, $right);

        return gmp_cmp($comparison, 0) == 0;
    }

    /**
     * implements https://www.secg.org/sec1-v2.pdf 2.3.4
     */
    public function fromXCoordinate(\GMP $x, bool $isEvenY): Point
    {
        $alpha = gmp_add(
            gmp_add(
                gmp_powm($x, gmp_init(3, 10), $this->curve->getP()),
                gmp_mul($this->curve->getA(), $x)
            ),
            $this->curve->getB()
        );

        $jacobiSymbol = gmp_jacobi($alpha, $this->curve->getP());
        if ($jacobiSymbol !== 1) {
            throw new Exception('No square root of alpha.');
        }

        /*
         * take the square root of alpha, while doing a (much cheaper) exponentiation
         *
         * observe that alpha^((p+1)/4) = y^((p+1)/2) = y^((p-1)/2) * y = y
         * (p+1)/4 is an integer, as for our prime p it holds that p mod 4 = 3
         * alpha = y^2 by the jacobi symbol check above that asserts y is a quadratic residue
         * y^((p-1)/2) = 1 by Euler's Criterion applies to the quadratic residue y
         */
        $const = gmp_div(gmp_add($this->curve->getP(), 1), 4);
        $beta = gmp_powm($alpha, $const, $this->curve->getP());

        $yp = $isEvenY ? gmp_init(0) : gmp_init(1);
        if (gmp_cmp(gmp_mod($beta, 2), $yp) === 0) {
            return new Point($x, $beta);
        } else {
            return new Point($x, gmp_sub($this->curve->getP(), $beta));
        }
    }

    /**
     * rules from https://www.secg.org/SEC1-Ver-1.0.pdf (2.2.1)
     */
    private function add(Point $a, Point $b): Point
    {
        // rule 1 & 2
        if ($a->isInfinity()) {
            return clone $b;
        } elseif ($b->isInfinity()) {
            return clone $a;
        }

        if (gmp_cmp($a->x, $b->x) === 0) {
            // rule 3
            if (gmp_cmp($b->y, $a->y) !== 0) {
                return Point::createInfinity();
            }

            // rule 5
            return $this->double($a);
        }

        // rule 4 (note that a / b = a * b^-1)
        $lambda = $this->field->mul(
            gmp_sub($b->y, $a->y),
            $this->field->invert(gmp_sub($b->x, $a->x))
        );

        $x = $this->field->sub(
            gmp_sub(
                gmp_pow($lambda, 2),
                $a->x
            ),
            $b->x
        );

        $y = $this->field->sub(
            gmp_mul(
                $lambda,
                gmp_sub($a->x, $x)
            ),
            $a->y
        );

        return new Point($x, $y);
    }

    private function double(Point $a): Point
    {
        if (gmp_cmp($a->y, 0) === 0) {
            return Point::createInfinity();
        }

        // rule 5 (note that a / b = a * b^-1)
        $lambda = $this->field->mul(
            gmp_add(
                gmp_mul(
                    gmp_init(3),
                    gmp_pow($a->x, 2)
                ),
                $this->curve->getA()
            ),
            $this->field->invert(
                gmp_mul(2, $a->y)
            )
        );

        $x = $this->field->sub(
            gmp_pow($lambda, 2),
            gmp_mul(2, $a->x)
        );

        $y = $this->field->sub(
            gmp_mul(
                $lambda,
                gmp_sub($a->x, $x)
            ),
            $a->y
        );

        return new Point($x, $y);
    }

    private function conditionalSwap(Point $a, Point $b, int $swapBit): void
    {
        $this->conditionalSwapScalar($a->x, $b->x, $swapBit, $this->field->getElementBitLength());
        $this->conditionalSwapScalar($a->y, $b->y, $swapBit, $this->field->getElementBitLength());
    }

    private function conditionalSwapScalar(GMP &$a, GMP &$b, int $swapBit, int $elementBitLength): void
    {
        // create a mask (note how it inverts the maskbit)
        $mask = gmp_init(str_repeat((string)(1 - $swapBit), $elementBitLength), 2);

        // if mask is 1, tempA = a, else temp = 0
        $tempA = gmp_and($a, $mask);
        $tempB = gmp_and($b, $mask);

        $a = gmp_xor($tempB, gmp_xor($a, $b)); // if mask is 1, then b XOR a XOR b = a, else 0 XOR a XOR b = a XOR b
        $b = gmp_xor($tempA, gmp_xor($a, $b)); // if mask is 1, then a XOR a XOR b = b, else 0 XOR a XOR b XOR b = a
        $a = gmp_xor($tempB, gmp_xor($a, $b)); // if mask is 1, then b XOR a XOR b = a, else 0 XOR a XOR b XOR a = b

        // hence if mask is 1 (= inverse of $swapBit), then no swap, else swap
    }

    /**
     * multiplication using the double-add-always
     */
    public function mul(Point $point, \GMP $factor): Point
    {
        $mulField = new PrimeField($this->curve->getN());

        // reduce factor once to ensure it is within our curve N bit size (and reduce computational effort)
        $reducedFactor = $mulField->mod($factor);

        // normalize to the element bit length to always execute the double-add loop a constant number of times
        $factorBits = gmp_strval($reducedFactor, 2);
        $normalizedFactorBits = str_pad($factorBits, $mulField->getElementBitLength(), '0', STR_PAD_LEFT);

        /**
         * how this works:
         * first, observe r[0] is infinity and r[1] our "real" point.
         * r[0] and r[1] are swapped iff the corresponding bit in $factor is set to 1,
         * hence if $j = 1, then the "real" point is added, else the "real" point is doubled
         */
        /** @var Point[] $r */
        $r = [Point::createInfinity(), clone $point];
        for ($i = 0; $i < $mulField->getElementBitLength(); $i++) {
            $j = (int)$normalizedFactorBits[$i];

            $this->conditionalSwap($r[0], $r[1], $j ^ 1);

            $r[0] = $this->add($r[0], $r[1]);
            $r[1] = $this->double($r[1]);

            $this->conditionalSwap($r[0], $r[1], $j ^ 1);
        }

        return $r[0];
    }
}

// secp256r1 curve from https://www.secg.org/sec2-v2.pdf (also known as NIST P-256).
$p = gmp_init('FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFF', 16);
$a = gmp_init('FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFC', 16);
$b = gmp_init('5AC635D8 AA3A93E7 B3EBBD55 769886BC 651D06B0 CC53B0F6 3BCE3C3E 27D2604B', 16);

$Gx = gmp_init('6B17D1F2 E12C4247 F8BCE6E5 63A440F2 77037D81 2DEB33A0 F4A13945 D898C296', 16);
$Gy = gmp_init('4FE342E2 FE1A7F9B 8EE7EB4A 7C0F9E16 2BCE3357 6B315ECE CBB64068 37BF51F5', 16);
$G = new Point($Gx, $Gy);

$n = gmp_init('FFFFFFFF 00000000 FFFFFFFF FFFFFFFF BCE6FAAD A7179E84 F3B9CAC2 FC632551', 16);
$curve = new Curve($p, $a, $b, $G, $n);
$math = new UnsafePrimeCurveMath($curve);
var_dump($math->isOnCurve($G)); // sanity check

// do diffie hellman key exchange
$aliceSecret = gmp_init('1421B466 CB12D4F1 298CF525 DE823345 B81B861F 25B5AA7B E86869F9 697C13D', 16);
$bobSecret = gmp_init('3CFFD9D8 3D5EF967 3432932D D70EC213 8D559C30 7EFBCFF6 0EB96EAB F08B0CBA', 16);

$alicePublicKey = $math->mul($curve->getG(), $aliceSecret);
$bobPublicKey = $math->mul($curve->getG(), $bobSecret);

$bobPublicKeyReconstructed = $math->fromXCoordinate($bobPublicKey->x, gmp_cmp(gmp_mod($bobPublicKey->y, 2), 0) === 0);
$aliceSharedKey = $math->mul($bobPublicKey, $aliceSecret);

$alicePublicKeyReconstructed = $math->fromXCoordinate($alicePublicKey->x, gmp_cmp(gmp_mod($alicePublicKey->y, 2), 0) === 0);
$bobSharedKey = $math->mul($alicePublicKey, $bobSecret);

var_dump($aliceSharedKey->equals($bobSharedKey));
var_dump(gmp_strval($aliceSharedKey->x, 16));
?>
--EXPECT--
bool(true)
bool(true)
string(64) "f480daf4f56a674c16944cda9e7c9fd0ab2813eae3a5935bf9e091cadb5c9ac3"
