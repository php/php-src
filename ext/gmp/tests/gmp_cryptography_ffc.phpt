--TEST--
Examples of the usage of gmp for finite field cryptography.
--DESCRIPTION--
This executes basic operations (addition, multiplication, inverse, exponentiation) as the "base operations".
Then, it performs a primality check, and finally diffie-hellman as the "application".
All operations are done in the 4096-bit MODP Group from RFC 3526: https://www.ietf.org/rfc/rfc3526.txt

Omitted are calculations on elliptic curves, which are also common, because of the complexity of these algorithms.
Elliptic curves generally operate on smaller values, so their use-case is somewhat covered here,
but curve calculations may need additional operations not used here.

Further, omitted is explicit demonstration of (public-key) encryption, commitments, zero-knowledge proofs or similar common applications.
However, the operation used in the diffie-hellman is at the core of all these other applications, hence these use-cases are implicitly covered, too.

$a, $b, and $c generated with
$random = gmp_random_range(0, $prime);
$randomHex = strtoupper(gmp_strval($random, 16));
echo chunk_split(chunk_split($randomHex, 8, " "), 54);
--EXTENSIONS--
gmp
--FILE--
<?php

$prime = gmp_init('
FFFFFFFF FFFFFFFF C90FDAA2 2168C234 C4C6628B 80DC1CD1
29024E08 8A67CC74 020BBEA6 3B139B22 514A0879 8E3404DD
EF9519B3 CD3A431B 302B0A6D F25F1437 4FE1356D 6D51C245
E485B576 625E7EC6 F44C42E9 A637ED6B 0BFF5CB6 F406B7ED
EE386BFB 5A899FA5 AE9F2411 7C4B1FE6 49286651 ECE45B3D
C2007CB8 A163BF05 98DA4836 1C55D39A 69163FA8 FD24CF5F
83655D23 DCA3AD96 1C62F356 208552BB 9ED52907 7096966D
670C354E 4ABC9804 F1746C08 CA237327 FFFFFFFF FFFFFFFF', 16);

$a = gmp_init('
BE774B3A 56642360 4B32CCF8 B721F519 E1FAD10F C8AB6109
D7B98E79 8A541A9B 5A747CC3 2927A1F9 AA8BFA3E 3F31858D
03DA94D5 B076FE11 35CBB577 70E8FF40 8B0F7E01 354C3436
1827ADF1 794E3C96 2BDBC8B9 6B894EF0 7CF67367 5F2B0B4F
0F6304FE A9F48EB9 D8D08C16 C00716F6 956AEEA5 9B7BB16A
7B29C225 1AF3988D 0F2381B2 6DDD130A 605BACD0 DE0104F9
71B1F8C9 43217768 D556A6BA AB2C5DED 69DC3CA3 79D6BBEA
8E9A8522 CCD6DD95 FA295909 C593D444 08A8832A A5429BFA', 16);

$b = gmp_init('
CA1CAE83 DBC72ACC 0BDA48CA 5AFF77D1 055F1CEA 0B4E3089
E2BC1661 F4878AF1 F28DE016 350F4182 ECF2DC26 691AAE10
BA6CB81B 375A1460 068CCB45 B948855B 3CE7FB9C 2754D50F
CE4B45F9 FF101B47 2F76A39B 707D1D0F F2EAA747 0E6AEB4B
37D582B3 2E724769 BB4D8088 FD2DB183 B67BB11F 3A61DF60
7C3029C9 33475CE6 9E3872EA 764AD8B4 CA42FDA0 04931B8C
046B2283 E06E291F 5CFE9369 7CC5A21B 13156554 59B11B21
CE206802 5738B90D EC4DB008 AA5B2BB3 1DCFE633 E05B91D9', 16);

$c = gmp_init('
8CA047D8 C5270CEF D43F181D 94901BF7 354BC803 AEFD1A1B
210B1500 C520C021 19CA1AE7 1422AD02 B326BBF3 19545111
FF5C284F 2A1083C5 31E9363E A98256F8 6F6B274C F44665F0
02FA45DD B3A1BF03 3A9BB662 EA5573AA 86BCD6A1 18E62597
A4B760A6 A393B406 265E4884 279B8C4E 209B3338 5A1A7D53
9D0F619A 18F967EF A0758D32 9E117136 F9ADDAE1 8AAF4718
837B4772 386C3B7D 988F8343 78F59991 81812F2D 93310E7E
9ED63DB7 5999C790 69D29570 08B7C39C 259BFFD2 74DC5E81', 16);

// check commutativity addition (a+b)+c = a+(b,c)
$d = gmp_mod(gmp_add($a, $b), $prime);
$left = gmp_mod(gmp_add($d, $c), $prime);
$d = gmp_mod(gmp_add($b, $c), $prime);
$right = gmp_mod(gmp_add($a, $d), $prime);
var_dump(gmp_cmp($left, $right) === 0);

// check distribution multiplication a(b+c) = ab + ac
$d = gmp_mod(gmp_add($b, $c), $prime);
$left = gmp_mod(gmp_mul($a, $d), $prime);
$d1 = gmp_mod(gmp_mul($a, $b), $prime);
$d2 = gmp_mod(gmp_mul($a, $c), $prime);
$right = gmp_mod(gmp_add($d1, $d2), $prime);
var_dump(gmp_cmp($left, $right) === 0);

// check inversion a * a^(-1) = 1
$inverse = gmp_invert($a, $prime);
$product = gmp_mod(gmp_mul($a, $inverse), $prime);
var_dump(gmp_cmp($product, 1) === 0);

// check exponentiation by group order a ^ (p-1) = 1
$groupOrder = gmp_sub($prime, 1);
$product = gmp_powm($a, $groupOrder, $prime);
var_dump(gmp_cmp($product, 1) === 0);

// check whether q is a safe prime (that is, p=(q-1)/2 is also prime)
$primeP = gmp_div($prime - 1, 2);
var_dump(gmp_prob_prime($primeP) > 0);

// diffie-hellman key exchange (g^a)^b = (g^b)^a
$generator = gmp_init(2);
$factorA = gmp_random_range(1, $primeP);
$factorB = gmp_random_range(1, $primeP);
$left = gmp_powm(gmp_powm($generator, $factorA, $primeP), $factorB, $primeP);
$right = gmp_powm(gmp_powm($generator, $factorB, $primeP), $factorA, $primeP);
var_dump(gmp_cmp($left, $right) === 0);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
